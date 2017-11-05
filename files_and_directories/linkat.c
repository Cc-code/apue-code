#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Use linkat(2) link a file(specified by argv[1]) to another file(specified by argv[2]). 
 * And verify that linkat(2) follows the symbolic link.
 */

#ifdef FOLLOW
#define MODE AT_SYMLINK_FOLLOW
#else
#define MODE 0
#endif

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <exist_path> <new_path>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (linkat(AT_FDCWD, argv[1], AT_FDCWD, argv[2], MODE) < 0) {
		perror("linkat error");
		exit(EXIT_FAILURE);
	}
	printf("linkat success\n");
	printf("%s - %s\n", argv[1], argv[2]);

	return 0;
}
