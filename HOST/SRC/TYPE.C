
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef struct macro_table {
	char *name;
	int len;
	void (*func)(uid_struct far *, uid_struct far *);
} macro_table;

void type(char far *filename, uid_struct far *uid, uid_struct far *head);
void rdmacro(char far *buf, uid_struct far *uid, uid_struct far *head);
void macro_id(uid_struct far *uid, uid_struct far *head);
void macro_command(uid_struct far *uid, uid_struct far *head);
void macro_version(uid_struct far *uid, uid_struct far *head);
void macro_rate(uid_struct far *uid, uid_struct far *head);
void macro_prate(uid_struct far *uid, uid_struct far *head);
void macro_ch(uid_struct far *uid, uid_struct far *head);
void macro_handle(uid_struct far *uid, uid_struct far *head);
void macro_name(uid_struct far *uid, uid_struct far *head);
void macro_lastlogout(uid_struct far *uid, uid_struct far *head);
void macro_lastread(uid_struct far *uid, uid_struct far *head);
void macro_login(uid_struct far *uid, uid_struct far *head);
void macro_comer(uid_struct far *uid, uid_struct far *head);
void macro_access(uid_struct far *uid, uid_struct far *head);
void macro_date(uid_struct far *uid, uid_struct far *head);
void macro_time(uid_struct far *uid, uid_struct far *head);
void macro_lmin(uid_struct far *uid, uid_struct far *head);
void macro_lsec(uid_struct far *uid, uid_struct far *head);
void macro_upl(uid_struct far *uid, uid_struct far *head);
void macro_downl(uid_struct far *uid, uid_struct far *head);
void macro_post(uid_struct far *uid, uid_struct far *head);
void macro_remain(uid_struct far *uid, uid_struct far *head);
void macro_tlimit(uid_struct far *uid, uid_struct far *head);
void macro_wait(uid_struct far *uid, uid_struct far *head);
void macro_anykey(uid_struct far *uid, uid_struct far *head);
void macro_lastlogin(uid_struct far *uid, uid_struct far *head);
void macro_sysstart(uid_struct far *uid, uid_struct far *head);

macro_table macrotab[] = {		/* í∑Ç¢èáÇ…Ç»ÇÁÇ◊ÇÈÇ±Ç∆ */
	{ "\\LASTLOGOUT", 11, macro_lastlogout },
	{ "\\LASTLOGIN",  10, macro_lastlogin  },
	{ "\\LASTREAD",   9,  macro_lastread   },
	{ "\\COMMAND",    8,  macro_command    },
	{ "\\VERSION",    8,  macro_version    },
	{ "\\ESTART",     7,  macro_sysstart   },
	{ "\\REMAIN",     7,  macro_remain     },
	{ "\\ACCESS",     7,  macro_access     },
	{ "?ANYKEY",      7,  macro_anykey     },
	{ "\\HANDLE",     7,  macro_handle     },
	{ "\\TLIMIT",     7,  macro_tlimit     },
	{ "\\SPEED",      6,  macro_prate      },
	{ "\\LOGIN",      6,  macro_login      },
	{ "\\COMER",      6,  macro_comer      },
	{ "\\DOWNL",      6,  macro_downl      },
	{ "\\POST",       5,  macro_post       },
	{ "\\RATE",       5,  macro_rate       },
	{ "\\DATE",       5,  macro_date       },
	{ "\\TIME",       5,  macro_time       },
	{ "\\LMIN",       5,  macro_lmin       },
	{ "\\LSEC",       5,  macro_lsec       },
	{ "\\NAME",       5,  macro_name       },
	{ "\\WAIT",       5,  macro_wait       },
	{ "\\UPL",        4,  macro_upl        },
	{ "\\ID",         3,  macro_id         },
	{ "\\CH",         3,  macro_ch         },
};


void main(void)
{
	char far *argv = getargv();
	uid_struct far *uid, far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	type(argv, uid, head);
}

void type(char far *filename, uid_struct far *uid, uid_struct far *head)
{
	int handle;
	char buf[128];

	if ((handle = open(filename, FA_READ)) == 0)
		return;
	while (fgets(buf, sizeof(buf), handle))
		rdmacro(buf, uid, head);

	close(handle);
}

void rdmacro(char far *buf, uid_struct far *uid, uid_struct far *head)
{
	int i;

	while (*buf != 0) {
		if (*buf == '\\' || *buf == '?') {
			for (i = 0; i < ELEMS(macrotab); i++) {
				if (strncmp(macrotab[i].name, buf, macrotab[i].len) == 0) {
					buf += macrotab[i].len;
					macrotab[i].func(uid, head);
					goto contin;
				}
			}
		}
		if (*buf != '\r')
			putchar(*buf);
		buf++;
	contin:
		;
	}
}

void macro_id(uid_struct far *uid, uid_struct far *head)
{
	print(uid->us.u.udat.id);
}

void macro_command(uid_struct far *uid, uid_struct far *head)
{
	print(uid->us.u.ucmd);
}

void macro_version(uid_struct far *uid, uid_struct far *head)
{
	spawn(P_WAIT, "version.dlm", "-numberonly");
}

void macro_rate(uid_struct far *uid, uid_struct far *head)
{
	char far *result;

	ioctl(uid->us.u.device_name, IOCTL_GETRESULT, &result);
	print(result);
}

void macro_prate(uid_struct far *uid, uid_struct far *head)
{
	long speed;

	ioctl(uid->us.u.device_name, IOCTL_GETSPEED, &speed);
	printf("%ld", speed);
}

void macro_ch(uid_struct far *uid, uid_struct far *head)
{
	printf("%d", getchnum(uid->us.u.device_name, head));
}

void macro_handle(uid_struct far *uid, uid_struct far *head)
{
	print(uid->us.u.udat.handle);
}

void macro_name(uid_struct far *uid, uid_struct far *head)
{
}

void macro_lastlogout(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, uid->us.u.udat.last_logout);
	printf("%02d/%02d/%02d %02d:%02d:%02d",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
}

void macro_lastread(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, uid->us.u.udat.last_msgread);
	printf("%02d/%02d/%02d %02d:%02d:%02d",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
}

void macro_login(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, uid->us.u.login_time);
	printf("%02d/%02d/%02d %02d:%02d:%02d",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
}

void macro_comer(uid_struct far *uid, uid_struct far *head)
{
	printf("%u", uid->us.u.total_access);
}

void macro_access(uid_struct far *uid, uid_struct far *head)
{
	printf("%u", uid->us.u.udat.access);
}

void macro_date(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, time());
	printf("%02d/%02d/%02d", t.year, t.month, t.day);
}

void macro_time(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, time());
	printf("%02d:%02d:%02d", t.hour, t.min, t.sec);
}

void macro_lmin(uid_struct far *uid, uid_struct far *head)
{
	long tdiff;
	ldiv_t d;

	tdiff = time() - uid->us.u.login_time;
	d = ldiv(tdiff, 60);
	printf("%ld", d.quot);
}

void macro_lsec(uid_struct far *uid, uid_struct far *head)
{
	long tdiff;
	ldiv_t d;

	tdiff = time() - uid->us.u.login_time;
	d = ldiv(tdiff, 60);
	printf("%ld", d.rem);
}

void macro_upl(uid_struct far *uid, uid_struct far *head)
{
	printf("%u", uid->us.u.udat.upload);
}

void macro_downl(uid_struct far *uid, uid_struct far *head)
{
	printf("%u", uid->us.u.udat.download);
}

void macro_post(uid_struct far *uid, uid_struct far *head)
{
	printf("%u", uid->us.u.udat.post);
}

void macro_remain(uid_struct far *uid, uid_struct far *head)
{
	long past;
	ldiv_t d;

	if (uid->us.u.tlimit == 65535)
		print("65535");
	else {
		past = time() - uid->us.u.login_time;
		d = ldiv(past, 60);

		if (uid->us.u.tlimit < d.quot)
			print("0");
		else
			printf("%u", uid->us.u.tlimit - d.quot);
	}
}

void macro_tlimit(uid_struct far *uid, uid_struct far *head)
{
	printf("%u", uid->us.u.tlimit);
}

void macro_wait(uid_struct far *uid, uid_struct far *head)
{
	waitsec(1, head);
}

void macro_anykey(uid_struct far *uid, uid_struct far *head)
{
	while (getchar() == -1)
		;
}

void macro_lastlogin(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, uid->us.u.udat.last_login);
	printf("%02d/%02d/%02d %02d:%02d:%02d",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
}

void macro_sysstart(uid_struct far *uid, uid_struct far *head)
{
	time_struct t;

	localtime(&t, head->us.p.start_time);
	printf("%02d/%02d/%02d %02d:%02d:%02d",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
}

