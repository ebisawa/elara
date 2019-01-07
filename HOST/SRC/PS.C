
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	unsigned pid, u;
	char far *p, s_stdin[9], s_stdout[9];
	uid_struct far *uid;

	puts(" pid ppid name     user      prio memory status wait / param stdin    stdout");

	for (pid = firstpid(); pid != 0; pid = nextpid(pid)) {
		if (getchar() != -1)
			break;

		uid = getuid(pid);
		u = getpflag(pid);

		/* pwatch ‚Ì uid ‚Í 0 ‚Å‚È‚¢‚Ì‚Å’ˆÓ */
		printf(" %3u%c%4u %-8s %-8s %5u %6ld ",
			pid, (u & P_DONTKILL) ? '*' : ' ', getppid(pid), getname(pid),
			(uid != 0 && uid->chstat == CS_USING) ? uid->us.u.udat.id : "",
			getprio(pid), getmuse(pid));
		switch (u = getstat(pid)) {
			case S_RUN:		print("run   ");        break;
			case S_WAIT:	print("wait  ");        break;
			case S_SLEEP:	print("sleep ");        break;
			case S_DELETED: print("killed");        break;
			default:		printf("%-3d   ", u);   break;
		}
		putchar(' ');
		switch (u = getwait1(pid)) {
			case 0:			print("none    ");      break;
			case W_USER:	print("user    ");      break;
			case W_CHILD:	print("child   ");      break;
			case W_MEMORY:	print("memory  ");      break;
			case W_FILE:	print("file    ");      break;
			default:		printf("%-3d     ", u); break;
		}
		printf("%4u ", getwait2(pid));
		getstdio(pid, s_stdin, s_stdout);
		printf("%-8s %-8s\n", (char far *)s_stdin, (char far *)s_stdout);
	}
}

