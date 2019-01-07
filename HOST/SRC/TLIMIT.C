
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

int Running;
unsigned PID;

void already_running(void);
void timeout(uid_struct far *uid, uid_struct far *head);

void main(void)
{
	int i;
	long t0, t1;
	uid_struct far *head;

	if (Running > 0)
		already_running();

	Running++;
	if ((head = getuid_n("pwatch")) == 0)
		return;

	PID = getpid();
	setuid(PID, 0);
	setstdio(PID, 0, 0);
	raise(W_CHILD, PID);
	kill_lock(1);

	for (;;) {
		t0 = head->us.p.now_time;
		for (i = 1; i <= head->us.p.maxusers; i++) {
			relcpu();
			if (head[i].us.u.logoff != 0)
				continue;

			switch (head[i].chstat) {
			case CS_USING:
				t1 = t0 - head[i].us.u.login_time;		/* Œo‰ßŽžŠÔ(•b) */
				if (t1 > lmul(head[i].us.u.tlimit, 60))
					timeout(&head[i], head);

				break;
			case CS_LOGIN:
				if (head[i].us.u.connect_time == 0)
					break;
				t1 = t0 - head[i].us.u.connect_time;
				if (t1 > 3 * 60)
					timeout(&head[i], head);

				break;
			}
		}
		if (head->us.p.aprio != 0)
			setprio(PID, head->us.p.aprio / head->us.p.maxusers);
	}
}

void already_running(void)
{
	exit();
}

void timeout(uid_struct far *uid, uid_struct far *head)
{
	int prio;

	prio = getprio(PID);
	setprio(PID, 1);
	iolimit(uid->us.u.device_name, PID);
	setstdio(PID, 0, uid->us.u.device_name);
	uid->us.u.logoff = DR_TIME_OUT;

	puts("\n\n## §ŒÀŽžŠÔ‚Å‚· ##");

	setstdio(PID, 0, 0);
	waitsec(1, head);
	disconnect_device(uid->us.u.device_name);
	iolimit(uid->us.u.device_name, 0);
	setprio(PID, prio);
}

