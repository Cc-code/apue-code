#include <stdio.h>
#include <stdlib.h>

/*
 * Use renameat(2) move a file(specified by argv[1]) to another file(specified by argv[2]). 
 * And verify that renameat(2) follows the symbolic link.
 */

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <old_file> <new_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (renameat(AT_FDCWD, argv[1], AT_FDCWD, argv[2]) < 0) {
		perror("renameat error");
		exit(EXIT_FAILURE);
	}
	printf("renameat success\n");
	printf("%s -> %s\n", argv[1], argv[2]);

	return 0;
}
