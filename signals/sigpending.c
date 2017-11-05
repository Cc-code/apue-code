#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef void (*sighandler_t)(int);

sighandler_t Signal(int signo, sighandler_t handler)
{
	struct sigaction	act, oact;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (signo != SIGALRM) {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;

	return oact.sa_handler;
}

void sig_quit(int signo)
{
	printf("catch SIGQUIT\n");

	if (Signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
		perror("Signal(SIGQUIT, SIG_DFL) error");
		exit(EXIT_FAILURE);
	}
}

int main()
{
	sigset_t	mask, oldmask, pendmask;

	if (Signal(SIGQUIT, sig_quit) == SIG_ERR) {
		perror("Signal(SIGQUIT) error");
		exit(EXIT_FAILURE);
	}

	/* Block SIGQUIT and sava current signal mask. */
	sigemptyset(&mask);
	sigaddset(&mask, SIGQUIT);
	if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0) {
		perror("sigprocmask#1 error");
		exit(EXIT_FAILURE);
	}

	sleep(3);

	if (sigpending(&pendmask) < 0) {
		perror("sigpending error");
		exit(EXIT_FAILURE);
	}
	if (sigismember(&pendmask, SIGQUIT))
		printf("SIGQUIT is pending\n");

	/* Restore signal mask which unblocks SIGQUIT. */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
		perror("sigprocmask#2 error");
		exit(EXIT_FAILURE);
	}
	printf("SIGQUIT unblocked\n");

	sleep(3);

	return 0;
}
