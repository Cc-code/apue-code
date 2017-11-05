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

sighandler_t Signal(int signo, sighandler_t handler)
{
	struct sigaction act, oact;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;

	return oact.sa_handler;
}

sighandler_t Signal_r(int signo, sighandler_t handler)
{
	struct sigaction act, oact;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART;
#endif

	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;

	return oact.sa_handler;
}

void sig_usr1(int signo)
{
//	psignal(signo, "catch signal");
}

void sig_usr2(int signo)
{
//	psignal(signo, "catch signal");

	if (Signal(SIGUSR2, sig_usr2) == SIG_ERR) {
		perror("Signal(SIGUSR2) error");
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
	if (Signal(SIGUSR1, sig_usr1) == SIG_ERR) {
		perror("Signal(SIGUSR1) error");
		exit(EXIT_FAILURE);
	}

	kill(getpid(), SIGUSR1);

	if ((handler = Signal(SIGUSR1, SIG_DFL)) == SIG_DFL) {
		printf("* Reset handler after invoked\n");
	} else if (handler == sig_usr1) {
		printf("* NOT reset handler after invoked\n");
	} else {
		printf("* UNKNOWN whether reset handler after invoked\n");
	}

	/* Signal are blocked while the handler is excuting. */
	if (Signal(SIGUSR2, sig_usr2) == SIG_ERR) {
		perror("Signal(SIGUSR2) error");
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
	
	/* Not set SA_RESTART flag for sa_flags And signal is SIGALRM */
	if (Signal(SIGALRM, sig_alrm) == SIG_ERR) {
		perror("Signal(SIGALRM) error");
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
			printf("* [DEFAULT] SIGALRM: blocking system calls NOT automatically restart\n");
		} else {
			perror("read error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (nread == 0) {
			fprintf(stderr, "child process occurred unknown error");
			exit(EXIT_FAILURE);
		} else {
			printf("* [DEFAULT] SIGALRM: blocking system calls automatically restart\n");
		}
	}
	close(fd[0]);

	/* Not set SA_RESTART flag for sa_flags And signal is not SIGALRM */
	if (Signal(SIGUSR1, sig_usr1) == SIG_ERR) {
		perror("Signal(SIGUSR1) error");
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
			printf("* [DEFAULT] OTHER:   blocking system calls NOT automatically restart\n");
		} else {
			perror("read error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (nread == 0) {
			fprintf(stderr, "child process occurred unknown error");
			exit(EXIT_FAILURE);
		} else {
			printf("* [DEFAULT] OTHER:   blocking system calls automatically restart\n");
		}
	}
	close(fd[0]);

	/* Set SA_RESTART flag for sa_flags And signal is SIGALRM */
	if (Signal_r(SIGALRM, sig_alrm) == SIG_ERR) {
		perror("Signal_r(SIGALRM) error");
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
			printf("* [RESTART] SIGALRM: blocking system calls NOT automatically restart\n");
		} else {
			perror("read error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (nread == 0) {
			fprintf(stderr, "child process occurred unknown error");
			exit(EXIT_FAILURE);
		} else {
			printf("* [RESTART] SIGALRM: blocking system calls automatically restart\n");
		}
	}
	close(fd[0]);

	/* Set SA_RESTART flag for sa_flags And signal is not SIGALRM */
	if (Signal_r(SIGUSR1, sig_usr1) == SIG_ERR) {
		perror("Signal_r(SIGUSR1) error");
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
			printf("* [RESTART] OTHER:   blocking system calls NOT automatically restart\n");
		} else {
			perror("read error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (nread == 0) {
			fprintf(stderr, "child process occurred unknown error");
			exit(EXIT_FAILURE);
		} else {
			printf("* [RESTART] OTHER:   blocking system calls automatically restart\n");
		}
	}
	close(fd[0]);

	/*
	 * Note that: For linux, if a signal interruptes read or write system call of pipe,
	 * results restart this system call always.
	 */

	return 0;
}
