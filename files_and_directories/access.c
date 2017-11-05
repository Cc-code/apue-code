#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Test the access permission for a file or directory specified by the command-line argument.
 */

int main(int argc, char *argv[])
{
	if (argc != 3 || strlen(argv[2]) != 1) {
		fprintf(stderr, "Usage: %s <filename> (r | w | x)\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int mode;

	switch (argv[2][0]) {
	case 'r':
		mode = R_OK;
		break;
	case 'w':
		mode = W_OK;
		break;
	case 'x':
		mode = X_OK;
		break;
	default:
		fprintf(stderr, "Usage: %s <filename> (r | w | x)\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (access(argv[1], mode) < 0) {
		perror("access error");
		exit(EXIT_FAILURE);
	}
	printf("access success\n");

	return 0;
}
