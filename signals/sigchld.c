#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void sig_chld(int signo)
{
	psignal(signo, "catch signal");

	if (wait(NULL) < 0) {
		perror("wait error");
		exit(EXIT_FAILURE);
	}
}

int main()
{
	pid_t		pid, pid_array[3];
	int			count = 0;

	/* Semantics of SIGCHLD which sepicified to SIG_IGN. */
	if (Signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		perror("Signal(SIGCHLD, SIG_IGN) error");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i != sizeof(pid_array) / sizeof(pid_t); ++i) {
		if ((pid_array[i] = fork()) < 0) {
			fprintf(stderr, "fork#%d error: %s\n", i, strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid_array[i] == 0) {
			return 0;
		}
	}

	while (wait(NULL) >= 0) {
		++count;
	}
	if (errno == ECHILD) {
		if (count != 0) {
			printf("* SIGCHLD: SIG_IGN is equivalent to SIG_DFL\n");
		} else {
			printf("* SIGCHLD: SIG_IGN throw away child exit status\n");
		}
	} else {
		perror("wait error");
		exit(EXIT_FAILURE);
	}

	/* Behavior of establishing handler for SIGCHLD. */
	if (Signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
		perror("Signal(SIGCHLD, SIG_DFL) error");
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		return 0;
	}

	if (waitid(P_PID, pid, NULL, WEXITED | WNOWAIT) < 0) {
		perror("waitid error");
		exit(EXIT_FAILURE);
	}

	if (Signal(SIGCHLD, sig_chld) == SIG_ERR) {
		perror("Signal(SIGCHLD) error");
		exit(EXIT_FAILURE);
	}

	if (wait(NULL) < 0) {
		if (errno == ECHILD) {
			printf("* SIGCHLD: handler invoked immediately after established\n");
		} else {
			perror("wait error");
			exit(EXIT_FAILURE);
		}
	} else {
		printf("* SIGCHLD: handler NOT invoked immediately after established\n");
	}

	return 0;
}
