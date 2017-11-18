#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

/*
 * Usage:
 * 	terminal 1. $ ./a.out lock r 5
 * 	terminal 2. $ ./a.out lock w 3
 *	termianl 3. $ ./a.out lock r 3
 *
 * Test system's behavior when a process is blocked while trying to write lock a range
 * of a file and additional read-lcok requests are made. Is the process requesting a
 * write lock starved by the processes read locking the file?
 */

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return fcntl(fd, cmd, &lock);
}

#define read_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

int main(int argc, char *argv[])
{
	int fd;
	int sleep_tm;
	struct flock lock, unlock;
	struct timespec tms;
	char buf[128];

	if (argc != 4 || strlen(argv[2]) != 1) {
		fprintf(stderr, "Usage: %s <filename> (r | w) <duration>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((fd = open(argv[1], O_CREAT | O_RDWR, 0644)) < 0) {
		perror("open error");
		exit(EXIT_FAILURE);
	}

	if (argv[2][0] == 'r')
		lock.l_type = F_RDLCK;
	else if (argv[2][0] == 'w')
		lock.l_type = F_WRLCK;
	else {
		fprintf(stderr, "Usage: %s <filename> (r | w) <duration>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 1;

	unlock.l_type = F_UNLCK;
	unlock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 1;

	sleep_tm = atoi(argv[3]);

	fcntl(fd, F_SETLKW, &lock);
	clock_gettime(CLOCK_REALTIME, &tms);
	strftime(buf, 128, "%r", localtime(&tms.tv_sec));
	printf("process %lu get %s lock at: %s\n", (unsigned long)getpid(),
			lock.l_type == F_RDLCK ? "read" : "write", buf);

	sleep(sleep_tm);
	fcntl(fd, F_SETLK, &unlock);
	clock_gettime(CLOCK_REALTIME, &tms);
	strftime(buf, 128, "%r", localtime(&tms.tv_sec));
	printf("process %lu release %s lock at: %s\n", (unsigned long)getpid(),
			lock.l_type == F_RDLCK ? "read" : "write", buf);

	return 0;
}
