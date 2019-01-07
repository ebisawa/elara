
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"

uid_struct far *getuid_n(char far *name)
{
	char far *procname;
	unsigned pid, npid;

	pid = firstpid(); npid = nextpid(pid);
	while (pid != 0) {
		procname = getname(pid);
		if (strcmp(procname, name) == 0)
			return getuid(pid);

		pid = npid;
		npid = nextpid(pid);
	}
	return 0;
}

