#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>

int main(int argc, char *argv[])
{
	int				ifd, ofd, ret;
	char			buf[BUFSIZ];
	ssize_t			nread;
	struct aiocb	oaiocb;
	const struct aiocb	*aiocblist[1];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
		exit(1);
	}

	if ((ifd = open(argv[1], O_RDONLY)) < 0) {
		perror("open error");
		exit(1);
	}
	if ((ofd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
		perror("open error");
		exit(1);
	}

	if ((nread = read(ifd, buf, BUFSIZ)) < 0) {
		perror("read error");
		exit(1);
	}

	memset(&oaiocb, 0, sizeof(oaiocb));
	oaiocb.aio_fildes = ofd;
	oaiocb.aio_offset = 0;
	oaiocb.aio_buf = buf;
	oaiocb.aio_nbytes = nread;

	if (aio_write(&oaiocb) < 0) {
		perror("aio_write error");
		exit(1);
	}

	aiocblist[0] = &oaiocb;
	if (aio_suspend(aiocblist, 1, NULL) < 0) {
		perror("aio_suspend error");
		exit(1);
	}

	if (aio_error(&oaiocb) != 0) {
		perror("aio_error error");
		exit(1);
	}

	if ((ret = aio_return(&oaiocb)) < 0) {
		perror("aio_return error");
		exit(1);
	}
	
	printf("asynchronous write completion\n");
	printf("aio_write operation return %d\n", ret);

	return 0;
}
