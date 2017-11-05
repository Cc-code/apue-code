#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	pid_t	pid;

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		if (execl("/home/Asu/Projects/apue/process_control/interpreter",
					"interpreter", "exec_arg1", "exec_arg2", NULL) < 0) {
			perror("execl error");
			exit(EXIT_FAILURE);
		}
	}

	if (waitpid(pid, NULL, 0) < 0) {
		perror("waitpid error");
		exit(EXIT_FAILURE);
	}

	return 0;
}
