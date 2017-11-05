#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef void (*sighandler_t)(int);

volatile sig_atomic_t	need_sleep = 1;
volatile unsigned long	left_to_sleep = 5;

void sig_usr1(int signo)
{
//	psignal(signo, "catch signal");
}

void sig_usr2(int signo)
{
//	psignal(signo, "catch signal");

	if (signal(SIGUSR2, sig_usr2) == SIG_ERR) {
		perror("signal(SIGUSR2) error");
		exit(EXIT_FAILURE);
	}

	if (need_sleep) {
		need_sleep = 0;
		left_to_sleep = sleep(left_to_sleep);
	}
}

void sig_alrm(int signo)
{
//	psignal(signo, "catch signal");
}

int main()
{
	sighandler_t	handler;
	pid_t			pid;
	int				fd[2];
	ssize_t			nread;
	char			ch = 0;

	/* Reset handler after invoked by recieved signal. */
	if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
		perror("signal(SIGUSR1) error");
		exit(EXIT_FAILURE);
	}

	kill(getpid(), SIGUSR1);

	if ((handler = signal(SIGUSR1, SIG_DFL)) == SIG_DFL) {
		printf("* Reset handler after invoked\n");
	} else if (handler == sig_usr1) {
		printf("* NOT reset handler after invoked\n");
	} else {
		printf("* UNKNOWN whether reset handler after invoked\n");
	}

	/* Signal are blocked while the handler is excuting. */
	if (signal(SIGUSR2, sig_usr2) == SIG_ERR) {
		perror("signal(SIGUSR2) error");
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		kill(getppid(), SIGUSR2);
		sleep(2);
		kill(getppid(), SIGUSR2);

		return 0;
	}

	while (waitpid(pid, NULL, 0) < 0) {
		if (errno == EINTR)
			continue;
		else {
			perror("waitpid error");
			exit(EXIT_FAILURE);
		}
	}

	if (left_to_sleep != 0) {
		printf("* NOT prevent received signal while the handler is excuting\n");
	} else {
		printf("* Prevent received signal while the handler is excuting\n");
	}

	/* Blocking system calls automatically restart if interrupted by handler. */
	if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
		perror("signal(SIGALRM) error");
		exit(EXIT_FAILURE);
	}

	if (pipe(fd) < 0) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		close(fd[0]);

		sleep(2);
		kill(getppid(), SIGALRM);

		write(fd[1], "a", 1);

		return 0;
	}

	close(fd[1]);
	if ((nread = read(fd[0], &ch, 1)) < 0) {
		if (errno == EINTR) {
			printf("* Signal SIGALRM: blocking system calls NOT automatically restart:\n");
		} else {
			perror("read error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (nread == 0) {
			fprintf(stderr, "child process occurred unknown error");
			exit(EXIT_FAILURE);
		} else {
			printf("* Signal SIGALRM: blocking system calls automatically restart\n");
		}
	}
	close(fd[0]);

	if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
		perror("signal(SIGUSR1) error");
		exit(EXIT_FAILURE);
	}

	if (pipe(fd) < 0) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		close(fd[0]);

		sleep(2);
		kill(getppid(), SIGUSR1);

		write(fd[1], "a", 1);

		return 0;
	}

	close(fd[1]);
	if ((nread = read(fd[0], &ch, 1)) < 0) {
		if (errno == EINTR) {
			printf("* Signal other:   blocking system calls NOT automatically restart\n");
		} else {
			perror("read error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (nread == 0) {
			fprintf(stderr, "child process occurred unknown error");
			exit(EXIT_FAILURE);
		} else {
			printf("* Signal OTHER:   blocking system calls automatically restart\n");
		}
	}
	close(fd[0]);

	/*
	 * Note that: For linux, if a signal interruptes read or write system call of pipe,
	 * results restart this system call always.
	 */

	return 0;
}
