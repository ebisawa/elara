
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

int Running;

void already_running(void);

void main(void)
{
	int i;
	unsigned pid, diff, accel1, accel2, m, prio = 1000;
	uid_struct far *head;
	long start_time;

	if (Running > 0)
		already_running();

	Running++;
	if ((head = getuid_n("pwatch")) == 0)
		return;
	pid = getpid();

	setuid(pid, 0);
	setstdio(pid, 0, 0);
	raise(W_CHILD, pid);
	kill_lock(1);

	accel1 = 0;
	accel2 = 0;

	for (;;) {
		head->us.p.now_time = start_time = time();
		head->us.p.now_time = time();
		if ((diff = head->us.p.now_time - start_time) > 0) {
			if (diff > 0 + 1)
				prio /= diff;
			else {
				m = 10 * accel1;
				if (m > prio)
					prio = 0;
				else
					prio -= m;
			}
			accel1++;
			accel2 = 0;
		} else {
			m += 100 * accel2;
			if (prio + m > PRIO_MAX)
				prio = PRIO_MAX;
			else
				prio += m;

			accel1 = 0;
			accel2++;
		}
		head->us.p.aprio = prio;
		setprio(pid, prio);
	}
}

void already_running(void)
{
	exit();
}

