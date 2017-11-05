#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Use link(2) link a file(specified by argv[1]) to another file(specified by argv[2]). 
 * And verify that link(2) follows the symbolic link.
 */

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <exist_path> <new_path>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (link(argv[1], argv[2]) < 0) {
		perror("link error");
		exit(EXIT_FAILURE);
	}
	printf("link success\n");
	printf("%s - %s\n", argv[1], argv[2]);

	return 0;
}
