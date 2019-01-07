
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "ioctl.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

#define F_NO_QUERY	0x0001
#define F_SILENT	0x0002
#define F_FORCE		0x0004

int Resident;
int Stop;

unsigned option(char far *argv, unsigned far *flags);
void wait_all_logout(uid_struct far *head, unsigned flags);
void stop_shutdown(uid_struct far *head);

void main(void)
{
	int q;
	char far *argv = getargv();
	unsigned retcode = 0, flags = 0;
	uid_struct far *uid, far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	retcode = option(argv, &flags);

	if (Resident) {
		if (flags & F_SILENT) {
			Stop = 1;
			head->us.p.status = SS_WAIT;
		} else {
			puts("全ユーザーの接続終了を待っています");
			if (cquery("作業を中断しますか", Q_NONE, uid) == Q_YES) {
				Stop = 1;
				head->us.p.status = SS_WAIT;
				putchar('\n');
				puts("-- 中止しました --");
			}
		}
		return;
	}
	if ((flags & F_SILENT) == 0) {
		printf("システムを停止します(エラーコード: %u)\n", retcode);
		if (flags & F_NO_QUERY)
			q = Q_YES;
		else
			q = cquery("本当によろしいですか", Q_NO, uid);

		putchar('\n');
	}
	if (q == Q_NO) {
		if ((flags & F_SILENT) == 0)
			puts("-- 中止しました --");
	} else {
		if ((flags & F_FORCE) == 0)
			wait_all_logout(head, flags);
		/* head->us.p.monitor_off = 1; */
		else {
			if ((flags & F_SILENT) == 0)
				puts("-- システム停止 --");
		}
		alldtr_off(head);
		shutdown(retcode);
	}
}

unsigned option(char far *argv, unsigned far *flags)
{
	unsigned retcode = 0;

	if (argv == 0)
		return 0;
	while (*argv != 0) {
		if (*argv == '-') {
			if (*++argv == 0)
				break;
			*argv = tolower(*argv);
			while (*argv != ' ' && *argv != 0) {
				switch(*argv) {
				case 'q':
					*flags |= F_NO_QUERY;
					break;
				case 's':
					*flags |= F_SILENT;
					break;
				case 'f':
					*flags |= F_FORCE;
					break;
				}
				argv++;
			}
		} else if (isdigit(*argv)) {
			retcode = atou(argv);
			while (isdigit(*argv) && *argv != 0)
				argv++;
		} else
			argv++;
	}
	return retcode;
}

void wait_all_logout(uid_struct far *head, unsigned flags)
{
	int i, using;
	unsigned pid = getpid();

	Resident++;
	if ((flags & F_SILENT) == 0)
		puts("-- 全ユーザーの接続終了を待ちます --");

	head->us.p.status = SS_SHUTDOWN;
	setstdio(pid, 0, 0);
	setuid(pid, 0);
	raise(W_CHILD, pid);

	for (;;) {
		using = 0;
		/* setprio(pid, head->us.p.aprio / head->us.p.maxusers); */
		setprio(pid, 1);
		if (Stop)
			stop_shutdown(head);
		for (i = 1; i <= head->us.p.maxusers; i++) {
			switch (head[i].chstat) {
			case CS_WAIT:
			case CS_SHORT:
			case CS_INIT:
			case CS_OFF:
				lineoff(head[i].us.u.device_name);
				head[i].chstat = CS_SHUTDOWN;
				head->us.p.monitor_redraw = 1;
				break;
			case CS_LOGIN:
			case CS_USING:
				using++;
				break;
			}
		}
		if (using == 0)
			return;
	}
}

void stop_shutdown(uid_struct far *head)
{
	int i;

	for (i = 0; i <= head->us.p.maxusers; i++) {
		if (head[i].chstat == CS_SHUTDOWN) {
			lineon(head[i].us.u.device_name);
			head[i].chstat = CS_WAIT;
			head->us.p.monitor_redraw = 1;
		}
	}
	Resident = 0;
	exit();
}

