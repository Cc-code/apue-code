#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Use symlink(2) make a symbolic link that refer to a file or directory. 
 */

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <actualpath> <sympath>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (symlink(argv[1], argv[2]) < 0) {
		perror("symlink error");
		exit(EXIT_FAILURE);
	}

	return 0;
}
