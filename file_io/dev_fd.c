#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

/*
 * Usage: ./a.out 111< /etc/hosts 222> out
 *
 * Open the file /dev/fd/n is equivalent to duplicating descriptor n,
 * assuming that descriptor n is open.
 *
 * For Linux, /dev/fd/n maps file descriptors into symbolic link pointing to
 * the underlying physical files.
 */

int main(int argc, char *argv[])
{
	int		ifd, ofd;
	ssize_t	n;
	char	buf[BUFSIZ];
	char	link_file[PATH_MAX + 1];
	struct stat statbuf;

	if ((ifd = open("/dev/fd/111", O_RDONLY)) < 0) {
		perror("open /dev/fd/111 error");
		exit(EXIT_FAILURE);
	}
	if ((ofd = open("/dev/fd/222", O_WRONLY)) < 0) {
		perror("open /dev/fd/222 error");
		exit(EXIT_FAILURE);
	}

	while ((n = read(ifd, buf, BUFSIZ)) > 0) {
		if (write(ofd, buf, n) != n) {
			perror("write #1 error");
			exit(EXIT_FAILURE);
		}
	}
	if (n < 0) {
		perror("read error");
		exit(EXIT_FAILURE);
	}

	if (lstat("/dev/fd/111", &statbuf) < 0) {
		perror("stat error");
		exit(EXIT_FAILURE);
	}
	if (S_ISLNK(statbuf.st_mode)) {
		if ((n = readlink("/dev/fd/111", link_file, PATH_MAX)) < 0) {
			perror("readlink errno");
			exit(EXIT_FAILURE);
		}
		link_file[n++] = '\n';
		if (write(ofd, link_file, n) != n) {
			perror("write #2 error");
			exit(EXIT_FAILURE);
		}
	} else {
		if (write(ofd, "/dev/fd/n is not symbolic link\n", 30) != 30) {
			perror("write #2 error");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
