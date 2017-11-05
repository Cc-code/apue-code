#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/*
 * Take a single command-line argument that specifieds a file descriptor
 * and prints a description of selected file flags for that descriptor.
 */

int main(int argc, char *argv[])
{
	int val;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <descriptor#>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0) {
		fprintf(stderr, "fcntl error for fd %d: %s\n", atoi(argv[1]), strerror(errno));
		exit(EXIT_FAILURE);
	}

	switch (val & O_ACCMODE) {
	case O_RDONLY:
		printf("read only");
		break;

	case O_WRONLY:
		printf("write only");
		break;

	case O_RDWR:
		printf("read write");
		break;

	default:
		fprintf(stderr, "unknown access mode\n");
		exit(EXIT_FAILURE);
	}

	if (val & O_APPEND)
		printf(", append");
	if (val & O_NONBLOCK)
		printf(", nonblocking");
	if (val & O_SYNC)
		printf(", synchronous writes");

#if !defined (_POSIX_C_SOURCE) && defined (O_FSYNC) && (OFSYNC != O_SYNC)
	if (val & O_FSYNC)
		printf(", synchronous writes");
#endif

	putchar('\n');
	return 0;
}
