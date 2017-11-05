#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 * Print the information of a file or directory specified by the command-line argument as ls(1).
 */

int main(int argc, char *argv[])
{
	struct stat buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &buf) < 0) {
		perror("stat error");
		exit(EXIT_FAILURE);
	}

	printf("%lu ", (unsigned long)buf.st_ino);

	if (S_ISREG(buf.st_mode))
		printf("-");
	else if (S_ISLNK(buf.st_mode))
		printf("l");
	else if (S_ISDIR(buf.st_mode))
		printf("d");
	else if (S_ISCHR(buf.st_mode))
		printf("c");
	else if (S_ISBLK(buf.st_mode))
		printf("b");
	else if (S_ISFIFO(buf.st_mode))
		printf("f");
	else if (S_ISSOCK(buf.st_mode))
		printf("s");
	else
		printf(" ");

	if (buf.st_mode & S_IRUSR)
		printf("r");
	else
		printf("-");
	if (buf.st_mode & S_IWUSR)
		printf("w");
	else
		printf("-");
	if (buf.st_mode & S_IXUSR || buf.st_mode & S_ISUID) {
		if (buf.st_mode & S_IXUSR && buf.st_mode & S_ISUID)
			printf("s");
		else if (buf.st_mode & S_IXUSR)
			printf("x");
		else
			printf("S");
	} else
		printf("-");
	if (buf.st_mode & S_IRGRP)
		printf("r");
	else
		printf("-");
	if (buf.st_mode & S_IWGRP)
		printf("w");
	else
		printf("-");
	if (buf.st_mode & S_IXGRP || buf.st_mode & S_ISGID) {
		if (buf.st_mode & S_IXGRP && buf.st_mode & S_ISGID)
			printf("s");
		else if (buf.st_mode & S_IXGRP)
			printf("x");
		else
			printf("S");
	} else
		printf("-");
	if (buf.st_mode & S_IROTH)
		printf("r");
	else
		printf("-");
	if (buf.st_mode & S_IWOTH)
		printf("w");
	else
		printf("-");
	if (buf.st_mode & S_IXOTH && buf.st_mode & S_ISVTX) {
		if (buf.st_mode & S_IXOTH && buf.st_mode & S_ISVTX)
			printf("t");
		else if (buf.st_mode & S_IXOTH)
			printf("x");
		else
			printf("T");
	} else
		printf("-");
	putchar('.');

	printf(" %lu ", (unsigned long)buf.st_nlink);

	printf(" %lu ", (unsigned long)buf.st_uid);
	printf(" %lu ", (unsigned long)buf.st_gid);

	printf(" %lu ", (unsigned long)buf.st_size);

	putchar('\n');
	return 0;
}
