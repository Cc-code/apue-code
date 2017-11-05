#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Use memory stream.
 */

#define BSZ 48

void show_mem(void *ptr, size_t len);

int main()
{
	FILE *fp;
	char buf[BSZ];

	memset(buf, 'a', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	if ((fp = fmemopen(buf, BSZ, "w+")) == NULL) {
		perror("fmemeopen failed");
		exit(EXIT_FAILURE);
	}
	printf("initial buffer contents: %s\n", buf);
	printf("before fprintf:");
	show_mem(buf, BSZ);
	fprintf(fp, "hello, world");
	printf("before flush: %s\n", buf);
	fflush(fp);
	printf("after fflush: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'b', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	fprintf(fp, "hello, world");
	fseek(fp, 0, SEEK_SET);
	printf("after fseek: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'c', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	fprintf(fp, "hello, world");
	fclose(fp);
	printf("after fclose: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	return 0;
}

void show_mem(void *ptr, size_t len)
{
	char *buf = (char *)ptr;
	for (size_t i = 0; i != len; ++i) {
		if (buf[i] == 0)
			putchar('-');
		else
			putchar(buf[i]);
	}
	putchar('\n');
}
