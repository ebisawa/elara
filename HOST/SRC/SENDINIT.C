
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void exitinit(uid_struct far *uid, uid_struct far *head);
void xgets(char far *buf);
void modeminit(uid_struct far *uid, uid_struct far *head);

void main(void)
{
	unsigned pid = getpid();
	uid_struct far *uid, far *head;
	char far *argv;

	if ((argv = getargv()) == 0)
		return;
	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid  = getuid_d(argv, head)) == 0)
		return;

	/*
	 * uid->chstat == CS_INIT だと、回線切断直後に着信が起きると
	 * login を２重起動することになってしまう。
	 *
	 * そのような場合には初期化を中断する、
	 */
	if (uid->chstat != CS_WAIT)
		return;							/* ２重起動防止 */

	uid->chstat = CS_INIT;
	head->us.p.monitor_redraw = 1;

	if (setstdio(pid, argv, argv) != 0)
		exitinit(uid, head);

	lineon(uid->us.u.device_name);		/* DTR が on でないと初期化できない */
	if (uid->us.u.initstr[0] != 0)
		modeminit(uid, head);

	exitinit(uid, head);
}

void exitinit(uid_struct far *uid, uid_struct far *head)
{
	uid->chstat = CS_WAIT;
	head->us.p.monitor_redraw = 1;
	exit();
}

void xgets(char far *buf)
{
	int c;

	for (;;) {
		while ((c = getchar()) == -1)
			;
		if (c == '\n')
			break;

		*buf++ = c;
	}
	*buf = 0;
}

void modeminit(uid_struct far *uid, uid_struct far *head)
{
	char far *p, far *q;
	char buf[INITSTR_MAX + 1], buf2[64];

	strcpy(buf, uid->us.u.initstr);
	p = buf;

	ioctl(uid->us.u.device_name, IOCTL_WRITEON, 0);
	while (p != 0) {
		waitsec(1, head);			/* １秒待つ */
		q = strspl(p, "\t ");
	again:
		puts(p);

		for (;;) {
			xgets(buf2);
			if (strcmp(buf2, "ERROR") == 0)
				goto again;
			if (strcmp(buf2, "OK") == 0) {
				ioctl(uid->us.u.device_name, IOCTL_CLEARBUF, 0);
				break;
			}
		}
		p = q;
	}
	ioctl(uid->us.u.device_name, IOCTL_WRITEOFF, 0);
}

