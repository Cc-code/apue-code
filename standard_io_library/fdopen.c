#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

/*
 * fdopen(2) may set O_APPEND flag.
 */

int main(int argc, char *argv[])
{
	FILE	*fp;
	int		fd, ch;
	ssize_t n;
	off_t	off1, off2;
	int		mode;
	char	buf[512] = " world!";

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((fd = open(argv[1], O_RDWR)) < 0) {
		perror("open error");
		exit(EXIT_FAILURE);
	}
	if ((mode = fcntl(fd, F_GETFL)) < 0) {
		perror("fcntl error");
		exit(EXIT_FAILURE);
	}
	printf("before fdopen\n");
	if (mode & O_APPEND) {
		printf("file is append mode\n");
	} else {
		printf("file is not append mdoe\n");
	}

	if ((fp = fdopen(fd, "a+")) == NULL) {
		perror("fdopen error");
		exit(EXIT_FAILURE);
	}
	if ((mode = fcntl(fd, F_GETFL)) < 0) {
		perror("fcntl error");
		exit(EXIT_FAILURE);
	}
	printf("after fdopen\n");
	if (mode & O_APPEND) {
		printf("file is append mode\n");
	} else {
		printf("file is not append mdoe\n");
	}

	if (write(fd, buf, strlen(buf)) != strlen(buf)) {
		perror("write error");
		exit(EXIT_FAILURE);
	}
	printf("write success\n");

	return 0;
}
