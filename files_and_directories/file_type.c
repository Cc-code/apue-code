#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/*
 * Print the type of a file or directory specified by the command-line argument.
 */

int main(int argc, char *argv[])
{
	int i;
	struct stat buf;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <path> ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	for (i = 1; i != argc; ++i) {
		printf("%s: ", argv[i]);
		if (lstat(argv[i], &buf) < 0) {
			perror("stat error");
			exit(EXIT_FAILURE);
		}
		
		if (S_ISREG(buf.st_mode)) {
			printf("regular\n");
		} else if (S_ISDIR(buf.st_mode)) {
			printf("directory\n");
		} else if (S_ISLNK(buf.st_mode)) {
			printf("symbolic link\n");
		} else if (S_ISCHR(buf.st_mode)) {
			printf("character special\n");
		} else if (S_ISBLK(buf.st_mode)) {
			printf("block special\n");
		} else if (S_ISSOCK(buf.st_mode)) {
			printf("socket\n");
		} else if (S_ISFIFO(buf.st_mode)) {
			printf("fifo\n");
		} else {
			printf("** unknow type **\n");
		}
	}

	return 0;
}
