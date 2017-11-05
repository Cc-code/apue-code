#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	pid_t		pid;
	int			status;
	siginfo_t	info;

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		return 0;
	}

	if (waitid(P_PID, pid, &info, WEXITED | WNOWAIT) < 0) {
		perror("waitid error");
		exit(EXIT_FAILURE);
	}

	if (wait(&status) < 0) {
		if (errno == ECHILD) {
			printf("waitable status of child had been taken away\n");
		} else {
			perror("wait error");
			exit(EXIT_FAILURE);
		}
	} else {
		printf("waitable status of child had not been taken away\n");
	}

	return 0;
}
