
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void waitsec(int sec, uid_struct far *head)
{
	unsigned pid, prio;
	long start_time;

	pid = getpid();
	prio = getprio(pid);
	setprio(pid, head->us.p.aprio * sec);

	start_time = head->us.p.now_time;
	while (head->us.p.now_time <= start_time + sec)
		relcpu();

	setprio(pid, prio);
}

