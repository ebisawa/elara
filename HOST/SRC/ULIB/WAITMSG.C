
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

unsigned waitmsg(unsigned timeout, uid_struct far *head)
{
	unsigned pid, prio, msgs = 0;
	long start_time;

	pid = getpid();
	prio = getprio(pid);
	setprio(pid, head->us.p.aprio);

	while (head->us.p.now_time <= start_time + timeout) {
		if ((msgs = checkmsg()) != 0)
			break;
	}
	setprio(pid, prio);

	return msgs;
}

