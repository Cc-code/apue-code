#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/*
 * Modify the access permissions of a file or directory specified by the command-line argument.
 */

#define ACCMODE S_IRWXU | S_IRGRP | S_IROTH

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (chmod(argv[1], ACCMODE) < 0) {
		perror("chmod error");
		exit(EXIT_FAILURE);
	}
	printf("chmod success\n");
	printf("new access mode: rwxr--r--\n");

	return 0;
}
