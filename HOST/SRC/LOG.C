
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

#define F_ALL		0x0001
#define F_SUPER		0x0002

unsigned option(char far *argv, uid_struct far *uid, uid_struct far *head, int far *count);
int logopen(uid_struct far *head);
void disp_connect_time(accesslog far *log);
void disp_disconnect_time(accesslog far *log);
void disp_connecting_time(accesslog far *log);

char *drtable[] = {
	"",
	"CARRIER DOWN",
	"TIME OUT",
	"CANNOT LOGIN",
	"LOGOFF",
	"DUPLICATE",
	"CHANNEL ERROR",
	"SYSTEM ERROR",
	"ABSOLUTE"
};

void main(void)
{
	int i, handle, count = 0;
	unsigned flags;
	long pos;
	accesslog log;
	uid_struct far *head, far *uid;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;
	if ((handle = logopen(head)) == 0)
		return;
	flags = option(getargv(), uid, head, &count);
	if (count == 0)
		count = 10;

	pos = seek(handle, 0L, FS_END) - sizeof(accesslog);

	if ((flags & F_SUPER) == 0)
		puts("- # - -  ID  -   - Handle -");

again:
	for (i = 0; i < count; i++) {
	redo:
		if (getchar() != -1)
			break;

		seek(handle, pos, FS_SET);
		if (read(&log, sizeof(log), handle) == 0)
			break;

		if ((flags & F_SUPER) == 0) {
			if (log.id[0] == 0 ||
				log.disconnect_reason == DR_CANNOT_LOGIN ||
	 			log.disconnect_reason == DR_DUPLICATE ||
				log.disconnect_reason == DR_CHANNEL_ERROR) {
				pos -= sizeof(accesslog);
				goto redo;
			}
			printf("%05u %-8s  (%s)\n",
							log.count, FAR(log.id), FAR(log.handle));
			goto next;
		}
		printf("%05u %-8s (%-16s)  ", log.count, FAR(log.id), FAR(log.handle));
		disp_connect_time(&log);

		if (log.disconnect_reason != 0) {
			disp_disconnect_time(&log);
			disp_connecting_time(&log);
		} else {
			if (log.connect_time >= head->us.p.start_time)
				print("still logged in");
			else
				print("system down");
		}
		putchar('\n');

		if (flags & F_SUPER) {
			printf("  channel %02d: ", log.channel);
			printf("post %3d, read %3d, mail %3d, up %3d, down %3d  ",
				log.d_hist.post, log.d_hist.read, log.d_hist.mail,
				log.d_hist.upload, log.d_hist.download);
			puts(drtable[log.disconnect_reason]);
		}
	next:
		pos -= sizeof(accesslog);
	}
	if (i == 10 && flags & F_ALL)
		goto again;

	close(handle);
}

unsigned option(char far *argv, uid_struct far *uid, uid_struct far *head, int far *count)
{
	unsigned flags = 0;

	if (argv == 0)
		return 0;
	while (*argv != 0) {
		if (*argv == '-') {
			if (*++argv == 0)
				break;
			*argv = tolower(*argv);
			while (*argv != ' ' && *argv != 0) {
				switch(*argv) {
				case 'a':
					flags |= F_ALL;
					break;
				case 's':
					if (checkopt("log_s", uid->us.u.udat.usrflag, head))
						flags |= F_SUPER;

					break;
				}
				argv++;
			}
		} else if (isdigit(*argv)) {
			*count = atou(argv);
			while (isdigit(*argv) && *argv != 0)
				argv++;
		} else
			argv++;
	}
	return flags;
}

int logopen(uid_struct far *head)
{
	return dopen(head->us.p.config->dir.logdir, "log.dat", FA_READ);
}

void disp_connect_time(accesslog far *log)
{
	time_struct t;

	localtime(&t, log->connect_time);
	printf("%02d/%02d/%02d %02d:%02d:%02d - ",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
}

void disp_disconnect_time(accesslog far *log)
{
	time_struct t2;

	localtime(&t2, log->disconnect_time);

	printf("%02d:%02d:%02d ", t2.hour, t2.min, t2.sec);
}

