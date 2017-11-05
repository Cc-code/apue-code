#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>

static void pr_times(clock_t, const struct tms *, const struct tms *);
static void do_cmd(const char *);
static void pr_exit(int);

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
	for (int i = 1; i != argc; ++i)
		do_cmd(argv[i]);

	return 0;
}

static void do_cmd(const char *cmd)
{
	struct tms	tmsstart, tmsend;
	clock_t		start, end;
	int			status;

	printf("\ncommand: %s\n", cmd);

	if ((start = times(&tmsstart)) == -1) {
		perror("#1 times error");
		exit(EXIT_FAILURE);
	}

	if ((status = system(cmd)) < 0) {
		perror("system() error");
		exit(EXIT_FAILURE);
	}

	if ((end = times(&tmsend)) == -1) {
		perror("#2 times error");
		exit(EXIT_FAILURE);
	}

	pr_times(end - start, &tmsstart, &tmsend);
	pr_exit(status);
}

static void pr_times(clock_t real, const struct tms *tmsstart, const struct tms *tmsend)
{
	static long	clktck = 0;

	if (clktck == 0) {
		if ((clktck = sysconf(_SC_CLK_TCK)) < 0) {
			perror("sysconf error");
			exit(EXIT_FAILURE);
		}
	}

	printf(" %-12s%7.2f\n", "real:", real / (double)clktck);
	printf(" %-12s%7.2f\n", "user:",
			(tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
	printf(" %-12s%7.2f\n", "sys:",
			(tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
	printf(" %-12s%7.2f\n", "child user:",
			(tmsend->tms_cutime - tmsstart->tms_cutime) / (double)clktck);
	printf(" %-12s%7.2f\n", "child sys:",
			(tmsend->tms_cstime - tmsstart->tms_cstime) / (double)clktck);
}

static void pr_exit(int status)
{
	if (WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
				WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",
				WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else
				"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
				WSTOPSIG(status));
}
