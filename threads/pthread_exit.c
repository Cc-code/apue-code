#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thr_fn1(void *arg)
{
	printf("thread 1 returning\n");
	return (void *)1;
}

void *thr_fn2(void *arg)
{
	printf("thread 2 returning\n");
	pthread_exit((void *)2);
}

void *thr_fn3(void *arg)
{
	printf("thread 3 canceled\n");
	pause();
	pthread_exit((void *)3);
}

void *thr_fn4(void *arg)
{
	printf("thread 4 detached\n");
	pause();
	pthread_exit((void *)4);
}

int main()
{
	int err;
	pthread_t	tid1, tid2, tid3, tid4;
	pthread_attr_t attr;
	void *tret;

	if ((err = pthread_attr_init(&attr)) != 0) {
		fprintf(stderr, "pthread_attr_init error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0) {
		fprintf(stderr, "pthread_attr_setdetachstate error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((err = pthread_create(&tid1, NULL, thr_fn1, NULL)) != 0) {
		fprintf(stderr, "pthread_create#1 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = pthread_create(&tid2, NULL, thr_fn2, NULL)) != 0) {
		fprintf(stderr, "pthread_create#2 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = pthread_create(&tid3, NULL, thr_fn3, NULL)) != 0) {
		fprintf(stderr, "pthread_create#3 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = pthread_create(&tid4, &attr, thr_fn4, NULL)) != 0) {
		fprintf(stderr, "pthread_create#4 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((err = pthread_join(tid1, &tret)) != 0) {
		fprintf(stderr, "pthread_join#1 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	printf("thread 1 exit code %ld\n", (long)tret);

	if ((err = pthread_join(tid2, &tret)) != 0) {
		fprintf(stderr, "pthread_join#2 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	printf("thread 2 exit code %ld\n", (long)tret);

	if ((err = pthread_cancel(tid3)) != 0) {
		fprintf(stderr, "pthread_cancel error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = pthread_join(tid3, &tret)) != 0) {
		fprintf(stderr, "pthread_join#3 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	printf("thread 3 exit code %ld%s\n", (long)tret,
			(void *)tret == PTHREAD_CANCELED ? " (PTHREAD_CANCELED)" : "");

	if ((err = pthread_join(tid4, &tret)) != 0) {
		fprintf(stderr, "pthread_join#4 error: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
	printf("thread 4 exit code %ld\n", (long)tret);

	return 0;
}
