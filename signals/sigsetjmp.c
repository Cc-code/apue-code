#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

volatile sig_atomic_t	canjump;
sigjmp_buf				jmpbuf;

typedef void (*sighandler_t)(int);

void sig_alrm(int);
void sig_usr1(int);
sighandler_t Signal(int, sighandler_t);
void pr_mask(const char *);

int main(int argc, char *argv[])
{
	int option;

	if ((argc != 2) || ((option = atoi(argv[1])) != 0 && option != 1)) {
		fprintf(stderr, "Usage: %s [0|1]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (Signal(SIGUSR1, sig_usr1) == SIG_ERR) {
		perror("Signal(SIGUSR1) error");
		exit(EXIT_FAILURE);
	}
	if (Signal(SIGALRM, sig_alrm) == SIG_ERR) {
		perror("Signal(SIGALRM) error");
		exit(EXIT_FAILURE);
	}

	if (sigsetjmp(jmpbuf, option)) {
		pr_mask("ending main");

		return 0;
	}
	canjump = 1;
	
	for ( ; ; )
		pause();

	return 0;
}

void sig_alrm(int signo)
{
	pr_mask("in sig_alrm");
}

void sig_usr1(int signo)
{
	time_t	starttime;

	if (jmpbuf == 0)
		return;

	pr_mask("starting sig_usr1");

	alarm(3);
	starttime = time(NULL);
	for ( ; ; )
		if (time(NULL) > starttime + 5)
			break;

	pr_mask("finishing sig_usr1");

	canjump = 0;
	siglongjmp(jmpbuf, 1);
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

void pr_mask(const char *str)
{
	sigset_t	mask;

	if (sigprocmask(0, NULL, &mask) < 0) {
		perror("sigprocmask error");
		exit(EXIT_FAILURE);
	} else {
		printf("%s:\n", str);
		if (sigismember(&mask, SIGABRT))
			printf(" SIGABRT");
		if (sigismember(&mask, SIGALRM))
			printf(" SIGALRM");
		if (sigismember(&mask, SIGBUS))
			printf(" SIGBUS");
#ifdef SIGCACEL
		if (sigismember(&mask, SIGCACEL))
			printf(" SIGCACEL");
#endif
		if (sigismember(&mask, SIGCHLD))
			printf(" SIGCHLD");
		if (sigismember(&mask, SIGCONT))
			printf(" SIGCONT");
#ifdef SIGEMT
		if (sigismember(&mask, SIGEMT))
			printf(" SIGEMT");
#endif
		if (sigismember(&mask, SIGFPE))
			printf(" SIGFPE");
#ifdef SIGFREEZE
		if (sigismember(&mask, SIGFREEZE))
			printf(" SIGFREEZE");
#endif
		if (sigismember(&mask, SIGHUP))
			printf(" SIGHUP");
		if (sigismember(&mask, SIGILL))
			printf(" SIGILL");
#ifdef SIGINFO
		if (sigismember(&mask, SIGINFO))
			printf(" SIGINFO");
#endif
		if (sigismember(&mask, SIGINT))
			printf(" SIGINT");
		if (sigismember(&mask, SIGIO))
			printf(" SIGIO");
		if (sigismember(&mask, SIGIOT))
			printf(" SIGIOT");
#ifdef SIGJVM1
		if (sigismember(&mask, SIGJVM1))
			printf(" SIGJVM1");
#endif
#ifdef SIGJVM2
		if (sigismember(&mask, SIGJVM2))
			printf(" SIGJVM2");
#endif
		if (sigismember(&mask, SIGKILL))
			printf(" SIGKILL");
#ifdef SIGLOST
		if (sigismember(&mask, SIGLOST))
			printf(" SIGLOST");
#endif
#ifdef SIGLWP
		if (sigismember(&mask, SIGLWP))
			printf(" SIGLWP");
#endif
		if (sigismember(&mask, SIGPIPE))
			printf(" SIGPIPE");
#ifdef SIGPOLL
		if (sigismember(&mask, SIGPOLL))
			printf(" SIGPOLL");
#endif
		if (sigismember(&mask, SIGPROF))
			printf(" SIGPROF");
#ifdef SIGPWR
		if (sigismember(&mask, SIGPWR))
			printf(" SIGPWR");
#endif
		if (sigismember(&mask, SIGQUIT))
			printf(" SIGQUIT");
		if (sigismember(&mask, SIGSEGV))
			printf(" SIGSEGV");
#ifdef SIGSTKFLT
		if (sigismember(&mask, SIGSTKFLT))
			printf(" SIGSTKFLT");
#endif
		if (sigismember(&mask, SIGSTOP))
			printf(" SIGSTOP");
		if (sigismember(&mask, SIGSYS))
			printf(" SIGSYS");
		if (sigismember(&mask, SIGTERM))
			printf(" SIGTERM");
#ifdef SIGTHAW
		if (sigismember(&mask, SIGTHAW))
			printf(" SIGTHAW");
#endif
#ifdef SIGTHR
		if (sigismember(&mask, SIGTHR))
			printf(" SIGTHR");
#endif
		if (sigismember(&mask, SIGTRAP))
			printf(" SIGTRAP");
		if (sigismember(&mask, SIGTSTP))
			printf(" SIGTSTP");
		if (sigismember(&mask, SIGTTIN))
			printf(" SIGTTIN");
		if (sigismember(&mask, SIGTTOU))
			printf(" SIGTTOU");
		if (sigismember(&mask, SIGURG))
			printf(" SIGURG");
		if (sigismember(&mask, SIGUSR1))
			printf(" SIGUSR1");
		if (sigismember(&mask, SIGUSR2))
			printf(" SIGUSR2");
		if (sigismember(&mask, SIGVTALRM))
			printf(" SIGVTALRM");
#ifdef SIGWAITING
		if (sigismember(&mask, SIGWAITING))
			printf(" SIGWAITING");
#endif
		if (sigismember(&mask, SIGWINCH))
			printf(" SIGWINCH");
		if (sigismember(&mask, SIGXCPU))
			printf(" SIGXCPU");
		if (sigismember(&mask, SIGXFSZ))
			printf(" SIGXFSZ");
#ifdef SIGXRES
		if (sigismember(&mask, SIGXRES))
			printf(" SIGXRES");
#endif
		putchar('\n');
	}
}
