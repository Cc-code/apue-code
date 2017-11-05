#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	pid_t		pid;

	if ((pid = fork()) < 0) {
		perror("#1 fork error");
		exit(EXIT_FAILURE);
	} else if (pid != 0) {
		sleep(2);
		exit(2);
	}

	if ((pid = fork()) < 0) {
		perror("#2 fork error");
		exit(EXIT_FAILURE);
	} else if (pid != 0) {
		sleep(4);
		abort();
	}

	if ((pid = fork()) < 0) {
		perror("#3 fork error");
		exit(EXIT_FAILURE);
	} else if (pid != 0) {
		execl("/bin/dd", "dd", "if=/etc/passwd", "of=/dev/null", NULL);
		exit(7);
	}

	if ((pid = fork()) < 0) {
		perror("#4 fork error");
		exit(EXIT_FAILURE);
	} else if (pid != 0) {
		sleep(8);
		exit(0);
	}

	sleep(6);
	kill(getpid(), SIGKILL);
	exit(6);
}
