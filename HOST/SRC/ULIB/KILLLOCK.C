
#include "klib.h"

void kill_lock(int sw)
{
	unsigned flag, pid = getpid();

	flag = getpflag(pid);

	if (sw == 0)
		flag &= ~P_DONTKILL;
	else
		flag |= P_DONTKILL;

	setpflag(pid, flag);
}

