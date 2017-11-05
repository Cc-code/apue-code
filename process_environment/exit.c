#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void fun1()
{
	printf("this is fun1\n");
}

void fun2()
{
	printf("this is fun2\n");
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int	nr;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <filename> <nr>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	atexit(fun1);
	atexit(fun2);
	atexit(fun1);
	atexit(fun2);

	nr = atoi(argv[2]);

	if ((fp = fopen(argv[1], "w")) == NULL) {
		perror("fopen error");
		exit(EXIT_FAILURE);
	}
	printf("fopen %s success\n", argv[1]);

	fprintf(fp, "hello, world!\n");
	printf("write to %s success\n", argv[1]);

	if (nr < 0)
		_exit(0);
	else if (nr > 0)
		_Exit(0);
	else
		exit(0);
}
