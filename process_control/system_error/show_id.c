#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("uid: %lu, euid: %lu\n",
			(unsigned long)getuid(),
			(unsigned long)geteuid());

	return 0;
}
