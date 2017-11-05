#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * SYNC macro requirements for opening output file with status flag O_SYNC.
 *
 * FCNTL_SYNC macro requirements for opening output file without status flag O_SYNC,
 * but then invoked fcntl to set this flag.
 *
 * For linux, fcntl(O_SYNC) doesn't work.
 */

#define BUFSIZE 4096

int main(int argc, char *argv[])
{
	ssize_t n;
	char 	buf[BUFSIZE];
	int 	ifd, ofd;
	int		flags, mode;

	if (argc != 3) {
		fprintf(stderr, "%s <infile> <outfile>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mode = O_RDWR | O_CREAT | O_TRUNC;

#ifdef SYNC
	mode |= O_SYNC;
#endif

	if ((ifd = open(argv[1], O_RDWR)) < 0) {
		perror("open input file error");
		exit(EXIT_FAILURE);
	}
	if ((ofd = open(argv[2], mode)) < 0) {
		perror("open output file error");
		exit(EXIT_FAILURE);
	}

#ifdef FCNTL_SYNC
	if ((flags = fcntl(ofd, F_GETFL, 0)) < 0) {
		perror("fcntl(F_GETFL) error");
		exit(EXIT_FAILURE);
	}

	flags |= O_SYNC;

	if (fcntl(ofd, F_SETFL, flags) < 0) {
		perror("fcntl(F_SETFL) error");
		exit(EXIT_FAILURE);
	}
#endif

	while ((n = read(ifd, buf, BUFSIZE)) > 0) {
		if (write(ofd, buf, n) != n) {
			perror("write error");
			exit(EXIT_FAILURE);
		}
	}
	if (n < 0) {
		perror("read error");
		exit(EXIT_FAILURE);
	}

	return 0;
}
