#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Usage: 
 * 		1. $ ./a.out < /etc/services > temp.file
 * 		2. $ ./a.out < /etc/services
 *
 * The standard output is set to be nonblocking.
 * If the standard output is a regular file, we expect the write to be executed once.
 * If the standard output is a terminal, we expect the write to return a partial count
 * sometimes and an error at other times.
 */

char buf[500000];

int main()
{
	ssize_t ntowrite, nwrite;
	char	*ptr;
	int		flags;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %ld bytes\n", ntowrite);

	flags = fcntl(STDOUT_FILENO, F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(STDOUT_FILENO, F_SETFL, flags) < 0) {
		perror("fcntl error");
		exit(EXIT_FAILURE);
	}

	ptr = buf;
	while (ntowrite > 0) {
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %ld, errno = %d\n", nwrite, errno);

		if (nwrite > 0) {
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}

	return 0;
}
