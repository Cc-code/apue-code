#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t	ntid;

void printids(const char *s)
{
	pid_t		pid;
	pthread_t	tid;

	pid = getpid();
	tid = pthread_self();
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
			(unsigned long)tid, (unsigned long)tid);
}

void *thr_fn(void *arg)
{
	printids("new thread: ");
	return (void *)0;
}

int main()
{
	int err, *retval;
	
	if ((err = pthread_create(&ntid, NULL, thr_fn, NULL)) != 0) {
		fprintf(stderr, "pthread_create error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	printids("main thread: ");

	sleep(2);

	return 0;
}
