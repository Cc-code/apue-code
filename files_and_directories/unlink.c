#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/*
 * Only when the link count reaches 0 can the contents of the file be deleted. One
 * other condition prevents the contents of a file from being deleted: as long as some
 * process has the file open, its contents will not be deleted. When a file closed, the
 * kernel first checks the count of the number of processes that have the file open. If
 * this count has reached 0, the kernel then checks the link count; if it is 0, the file's
 * contents are deleted.
 */

#define FILE_SIZE 102400000		/* 100M */

void sig_handler(int signo)
{
	psignal(signo, "catuch signal");
}

int main(int argc, char *argv[])
{
	int		fd, i, count = FILE_SIZE / BUFSIZ;
	char	buf[BUFSIZ];
	struct sigaction act;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}

	if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC)) < 0) {
		fprintf(stderr, "open(%s) error: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("open %s success\n", argv[1]);

	for (i = 0; i != count; ++i) {
		if (write(fd, buf, BUFSIZ) != BUFSIZ) {
			perror("write error");
			exit(EXIT_FAILURE);
		}
	}
	printf("write %s success\n", argv[1]);

	/*
	 * Execute 'df -h' show file system disk space.
	 */ 
	pause();

	unlink(argv[1]);
	printf("unlink %s success\n", argv[1]);

	/*
	 * Execute 'df -h' show file system disk space.
	 * Create a file with the same name as the unlinked file.
	 */
	pause();	

	printf("process exit\n");
	
	return 0;
}
