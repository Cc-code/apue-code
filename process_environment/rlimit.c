#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>

#define doit(name) pr_limits(#name, name)

static void pr_limits(const char *, int);

int main()
{
#ifdef RLIMIT_AS
	doit(RLIMIT_AS);
#endif

	doit(RLIMIT_CORE);
	doit(RLIMIT_CPU);
	doit(RLIMIT_DATA);
	doit(RLIMIT_FSIZE);
	
#ifdef RLIMIT_MEMLOCK
	doit(RLIMIT_MEMLOCK);
#endif

#ifdef RLIMIT_MSGQUEUE
	doit(RLIMIT_MSGQUEUE);
#endif

#ifdef RLIMIT_NICE
	doit(RLIMIT_NICE);
#endif

	doit(RLIMIT_NOFILE);

#ifdef RLIMIT_NPROC
	doit(RLIMIT_NPROC);
#endif

#ifdef RLIMIT_NPTS
	doit(RLIMIT_NPTS);
#endif

#ifdef RLIMIT_RSS
	doit(RLIMIT_RSS);
#endif

#ifdef RLIMIT_SBSIZE
	doit(RLIMIT_SBSIZE);
#endif

#ifdef RLIMIT_SIGPENDING
	doit(RLIMIT_SIGPENDING);
#endif

	doit(RLIMIT_STACK);

#ifdef RLIMIT_SWAP
	doit(RLIMIT_SWAP);
#endif

#ifdef RLIMIT_VMEM
	doit(RLIMIT_VMEM);
#endif

	return 0;
}

static void pr_limits(const char *name, int resource)
{
	struct rlimit		limit;
	unsigned long long	lim;

	if (getrlimit(resource, &limit) < 0) {
		fprintf(stderr, "getrlimit error for %s: %s\n", name, strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("%-18s ", name);

	if (limit.rlim_cur == RLIM_INFINITY) {
		printf("(infinite) ");
	} else {
		lim = limit.rlim_cur;
		printf("%10lld ", lim);
	}
	if (limit.rlim_max == RLIM_INFINITY) {
		printf("(infinite)");
	} else {
		lim = limit.rlim_max;
		printf("%10lld", lim);
	}
	putchar((int)'\n');
}
