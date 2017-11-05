#include <stdio.h>
#include <stdlib.h>

/*
 * Use tmpnam(3) and tmpfile(3).
 */
#define MAXLINE 512

int main(int argc, char *argv[])
{
	char name[L_tmpnam], line[MAXLINE];
	FILE *fp;

	printf("%s\n", tmpnam(NULL));

	tmpnam(name);
	printf("%s\n", name);

	if ((fp = tmpfile()) == NULL) {
		perror("tmpfile error");
		exit(EXIT_FAILURE);
	}
	fputs("one line of output\n", fp);
	rewind(fp);
	if (fgets(line, sizeof(line), fp) == NULL) {
		perror("fgets error");
		exit(EXIT_FAILURE);
	}
	fputs(line, stdout);

	return 0;
}
