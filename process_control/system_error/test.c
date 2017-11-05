#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s <command>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (system(argv[1]) < 0) {
		perror("system error");
		exit(EXIT_FAILURE);
	}

	return 0;
}
