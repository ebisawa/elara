
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	unsigned pid, prio;
	long l, start_time;

	pid = getpid();
	prio = getprio(pid);
	setprio(pid, 10);

	start_time = time();
	while (time() == start_time)
		;

	start_time = time();
	for (l = 0; time() == start_time; l++)
		;

	setprio(pid, prio);
	printf("Power: %ld\n", l);
}

