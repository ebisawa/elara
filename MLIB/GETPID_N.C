
#include "klib.h"
#include "mlib.h"

unsigned getpid_n(char far *name)
{
	char far *procname;
	unsigned pid, npid;

	pid = firstpid(); npid = nextpid(pid);
	while (pid != 0) {
		procname = getname(pid);
		if (strcmp(procname, name) == 0)
			return pid;

		pid = npid;
		npid = nextpid(pid);
	}
	return 0;
}

