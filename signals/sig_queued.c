#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

sig_atomic_t	sigcount;
sig_atomic_t	waitflag = 1;
sigset_t		mask, omask, zeromask;
volatile sig_atomic_t	sigflag;

typedef void (*sighandler_t)(int);

void sig_quit(int signo);
void sig_usr(int signo);
sighandler_t Signal(int, sighandler_t);
void TELL_WAIT();
void TELL_CHILD(pid_t);
void TELL_PARENT(pid_t);
void WAIT_CHILD();
void WAIT_PARENT();

int main()
{
	pid_t	pid;

	if (Signal(SIGQUIT, sig_quit) == SIG_ERR) {
		perror("Signal(SIGQUIT) error");
		exit(EXIT_FAILURE);
	}

	TELL_WAIT();

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		kill(getppid(), SIGQUIT);

		/*
		 * For Linux and Solaris, rapid  generating of signal results this signal is
		 * delivered only once to the process when it's unblocked.
		 */
		sleep(1);

		kill(getppid(), SIGQUIT);
		sleep(1);
		kill(getppid(), SIGQUIT);
		sleep(1);
		kill(getppid(), SIGQUIT);
		sleep(1);

		TELL_PARENT(getppid());
	} else {
		sleep(5);

		if (sigcount == 2)
			printf("Signal is NOT queued\n");
		else if (sigcount == 4)
			printf("Signal is queued\n");
		else {
			printf("Unknown error in handler\n");
			printf("sigcount: %d\n", sigcount);
		}
	}

	return 0;
}

void sig_quit(int signo)
{
	++sigcount;
	if (waitflag) {
		waitflag = 0;
		WAIT_CHILD();
	}
}

void sig_usr(int signo)
{
	sigflag = 1;
}

sighandler_t Signal(int signo, sighandler_t handler)
{
	struct sigaction	act, oact;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPTER
		act.act_flags |= SA_INTERRUPTER;
#endif
	} else {
#ifdef SA_RESTART
		act.act_flags |= SA_RESTART;
#endif
	}

	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;

	return oact.sa_handler;
}

void TELL_WAIT()
{
	if (Signal(SIGUSR1, sig_usr) == SIG_ERR) {
		perror("Signal(SIGUSR1) error");
		exit(EXIT_FAILURE);
	}
	if (Signal(SIGUSR2, sig_usr) == SIG_ERR) {
		perror("Signal(SIGUSR2) error");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&zeromask);
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);

	if (sigprocmask(SIG_BLOCK, &mask, &omask) < 0) {
		perror("SIG_BLOCK error");
		exit(EXIT_FAILURE);
	}
}

void TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);
}

void TELL_PARENT(pid_t pid)
{
	kill(pid, SIGUSR2);
}

void WAIT_CHILD()
{
	while (sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	if (sigprocmask(SIG_SETMASK, &omask, NULL) < 0) {
		perror("SIG_SETMASK error");
		exit(EXIT_FAILURE);
	}
}

void WAIT_PARENT()
{
	while (sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	if (sigprocmask(SIG_SETMASK, &omask, NULL) < 0) {
		perror("SIG_SETMASK error");
		exit(EXIT_FAILURE);
	}
}
