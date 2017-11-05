#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
	int		priority;

	if ((priority = getpriority(PRIO_PROCESS, 0)) == -1 && errno != 0) {
		perror("getpriority error");
		exit(EXIT_FAILURE);
	}
	printf("current priority: %d\n", priority);

	if (setpriority(PRIO_PROCESS, 0, priority + 5) < 0) {
		perror("#1 setpriority error");
		exit(EXIT_FAILURE);
	}
	printf("setpriority %d success\n", priority + 5);
	
	errno = 0;
	if ((priority = getpriority(PRIO_PROCESS, 0)) == -1 && errno != 0) {
		perror("getpriority error");
		exit(EXIT_FAILURE);
	}
	printf("current priority: %d\n", priority);

	if (setpriority(PRIO_PROCESS, 0, priority + 10) < 0) {
		perror("#2 setpriority error");
		exit(EXIT_FAILURE);
	}
	printf("setpriority %d success\n", priority + 10);
	
	errno = 0;
	if ((priority = getpriority(PRIO_PROCESS, 0)) == -1 && errno != 0) {
		perror("getpriority error");
		exit(EXIT_FAILURE);
	}
	printf("current priority: %d\n", priority);

	return 0;
}
