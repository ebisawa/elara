
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
	int handle;
	char buf[35];
	uid_struct far *uid, far *head;
	boardlist blist;

	if ((uid = getuid(getpid())) == 0)
		return;
	if ((head = getuid_n("pwatch")) == 0)
		return;

	handle = dopen(head->us.p.config->dir.bbsdir, "blist.dat", FA_READ);
	if (handle == 0)
		return;
	while (read(&blist, sizeof(blist), handle)) {
		if ((blist.read & uid->us.u.udat.usrflag) == 0)
			continue;
		printf(" (R%c%c) ",
			(blist.write    & uid->us.u.udat.usrflag) ? 'W' : ' ',
			(blist.basenote & uid->us.u.udat.usrflag) ? 'B' : ' ');
		sprintf(buf, "<%s>", FAR(blist.realname));
		printf("%-32.32s %-.38s\n", FAR(buf), FAR(blist.desc));
	}
	close(handle);
}

