
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void dispmenu(uid_struct far *head, long far *flagp, char far *msg);
unsigned long getbdat(int flagno);
int isfset(long far *flagp, int flagno);
void revflag(long far *flagp, int flagno);

void main(void)
{
	char far *argv, far *msg, buf[3];
	unsigned num;
	long far *flagp;
	uid_struct far *head;

	if ((argv = getargv()) == 0)
		return;
	if ((head = getuid_n("pwatch")) == 0)
		return;

	msg = strspl(argv, " ");
	if ((flagp = str2farp(argv)) == 0)
		return;

	for (;;) {
		dispmenu(head, flagp, msg);
	again:
		print("î‘çÜ : ");
		if (gets(buf, sizeof(buf)) == 0)
			return;

		num = atou(buf);
		if (num < 1 || num > 32)
			goto again;

		revflag(flagp, num);
	}
}

void dispmenu(uid_struct far *head, long far *flagp, char far *msg)
{
	int i;
	flagdat_t far *flags = head->us.p.flagdat;

	putchar('\n');
	if (msg != 0)
		printf("< %s >\n\n", msg);

	for (i = 0; i < 16; i++) {
		printf("[%2d]%c%5u %-3s> %-20s  [%2d]%c%5u %-3s> %-20s\n",
			i + 1, (isfset(flagp, i + 1)) ? '*' : ' ',
			flags[i].tlimit, flags[i].ext, flags[i].flag_name,

			i + 17, (isfset(flagp, i + 17)) ? '*' : ' ',
			flags[i+16].tlimit,flags[i+16].ext,flags[i+16].flag_name);
	}
	putchar('\n');
}

unsigned long getbdat(int flagno)
{
	int i;
	unsigned long l = 0x80000000L;

	for (i = 0; i < flagno - 1; i++)
		l >>= 1;

	return l;
}
int isfset(long far *flagp, int flagno)
{
	return (*flagp & getbdat(flagno)) != 0;
}

void revflag(long far *flagp, int flagno)
{
	*flagp ^= getbdat(flagno);
}

