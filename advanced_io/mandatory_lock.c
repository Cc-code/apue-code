#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

/*
 * Determine whether system supports mandatory locking.
 */

static int fd1[2], fd2[2];

void TELL_WAIT();
void WAIT_PARENT();
void TELL_CHILD();

int main(int argc, char *argv[])
{
	int fd, flags;
	pid_t pid;
	char buf[5];
	struct stat statbuf;
	struct flock lock;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
		perror("open error");
		exit(EXIT_FAILURE);
	}
	if (write(fd, "abcdef", 6) != 6) {
		perror("write error");
		exit(EXIT_FAILURE);
	}
 
	/* turn on set-group-ID and turn off group-execute */
	if (fstat(fd, &statbuf) < 0) {
		perror("fstat error");
		exit(EXIT_FAILURE);
	}
	if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
		perror("fchmod error");
		exit(EXIT_FAILURE);
	}

	TELL_WAIT();

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		/* write lock entire file */
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		if (fcntl(fd, F_SETLK, &lock) < 0) {
			perror("write lock error");
			exit(EXIT_FAILURE);
		}

		TELL_CHILD();

		if (waitpid(pid, NULL, 0) < 0) {
			perror("waitpid error");
			exit(EXIT_FAILURE);
		}
	} else {
		WAIT_PARENT();

		if ((flags = fcntl(fd, F_GETFL)) < 0) {
			perror("fcntl error");
			exit(EXIT_FAILURE);
		}
		flags |= O_NONBLOCK;
		if (fcntl(fd, F_SETFL, flags)) {
			perror("fcntl error");
			exit(EXIT_FAILURE);
		}

		/* first let's see what error we get if region is locked */
		lock.l_type = F_RDLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		if (fcntl(fd, F_SETLK, &lock) != -1) {
			fprintf(stderr, "child: read lock succeeded\n");
			exit(EXIT_FAILURE);
		}
		printf("read lock of already-locked region returns %d\n", errno);

		/* now try to read the mandatory locked file */
		if (lseek(fd, 0, SEEK_SET) == -1) {
			perror("lseek error");
			exit(EXIT_FAILURE);
		}
		if (read(fd, buf, 2) < 0) {
			perror("read failed (mandatory locking works)");
		} else {
			printf("read OK (no mandatory locking), buf = %2.2s\n", buf);
		}
	}

	return 0;
}

void TELL_WAIT()
{
	if (pipe(fd1) < 0) {
		perror("pipe#1 error");
		exit(EXIT_FAILURE);
	}
	if (pipe(fd2) < 0) {
		perror("pipe#2 error");
		exit(EXIT_FAILURE);
	}
}

void TELL_CHILD()
{
	if (write(fd1[1], "p", 1) != 1) {
		perror("parent write error");
		exit(EXIT_FAILURE);
	}
}

void WAIT_PARENT()
{
	char ch;
	ssize_t n;

	if ((n = read(fd1[0], &ch, 1)) < 0) {
		perror("child read error");
		exit(EXIT_FAILURE);
	}
	if (n == 0) {
		fprintf(stderr, "parent has an unknown error\n");
		exit(EXIT_FAILURE);
	}
}
