#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

/*
 * MAP_ANON
 * 		The mapping is not backed by any file; its contents are initialized to be zero.
 * 		The fd required to be -1, and offset argument is ignored.
 */

#define SEM_NAME "/mysem"

int main(int argc, char *argv[])
{
	pid_t	pid;
	int		nloop, i;
	int		*ptr;
	sem_t	*mutex;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <#nloop>\n", argv[0]);
		exit(1);
	}
	nloop = atoi(argv[1]);

	if ((ptr = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0)) == MAP_FAILED) {
		perror("mmap error");
		exit(1);
	}

	if ((mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
		perror("sem_open error");
		exit(1);
	}
	sem_unlink(SEM_NAME);

	if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
		perror("setvbuf error");
		exit(1);
	}

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(1);
	} else if (pid == 0) {
		for (i = 0; i < nloop; ++i) {
			if (sem_wait(mutex) < 0) {
				perror("sem_wait error");
				exit(1);
			}
			printf("child: %d\n", (*ptr)++);
			if (sem_post(mutex) < 0) {
				perror("sem_post error");
				exit(1);
			}
		}
	} else {
		for (i = 0; i < nloop; ++i) {
			if (sem_wait(mutex) < 0) {
				perror("sem_wait error");
				exit(1);
			}
			printf("parent: %d\n", (*ptr)++);
			if (sem_post(mutex) < 0) {
				perror("sem_post error");
				exit(1);
			}
		}
	}

	return 0;
}
