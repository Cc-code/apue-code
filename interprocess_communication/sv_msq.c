#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>

/* MSGMAXSZ contains terminating null byte ('\0') */
#define MSGMAXSZ (BUFSIZ - sizeof(long))

struct mymsg {
	long type;
	char text[1];
};

#define IPC_MODE 0644

void thr_func(void *arg)
{
	int				msqid;
	struct mymsg 	*msgptr;

	msqid = *(int *)arg;
	msgptr = (struct mymsg *)malloc(BUFSIZ);

	if (msgptr == NULL) {
		perror("msgptr error");
		exit(1);
	}

	while (msgrcv(msqid, msgptr, MSGMAXSZ, 0, 0) >= 0) {
		printf("msgrcv: %s", msgptr->text);
	}

	if (errno == EIDRM) {
		fprintf(stderr, "msgrcv over\n");
		free(msgptr);
	} else {
		perror("msgrcv error");
		exit(1);
	}
		
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	key_t			key;
	int				msqid, err;
	pthread_t		thread;
	struct mymsg	*msgptr;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <pathname> <project id>\n", argv[0]);
		exit(1);
	}

	if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
		perror("setvbuf error");
		exit(1);
	}

	if ((msgptr = (struct mymsg *)malloc(BUFSIZ)) == NULL) {
		perror("malloc error");
		exit(1);
	}

	if ((key = ftok(argv[1], atoi(argv[2]))) < 0) {
		fprintf(stderr, "ftok error\n");
		exit(1);
	}
	if ((msqid = msgget(key, IPC_CREAT | IPC_EXCL | IPC_MODE)) < 0) {
		perror("msgget error");
		exit(1);
	}

	if ((err = pthread_create(&thread, NULL, thr_func, &msqid)) != 0) {
		fprintf(stderr, "pthread_create error: %s\n", strerror(err));
		exit(1);
	}

	while (fgets(msgptr->text, MSGMAXSZ, stdin) != NULL) {
		msgptr->type = 1;

		if (msgsnd(msqid, msgptr, strlen(msgptr->text) + 1, 0) < 0) {
			perror("msgsnd error");
			exit(1);
		}
	}

	if (msgctl(msqid, IPC_RMID, NULL) < 0) {
		perror("msgctl(IPC_RMID) error");
		exit(1);
	}

	pthread_join(thread, NULL);
	
	return 0;
}
