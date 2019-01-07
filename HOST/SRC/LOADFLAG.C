
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

int Running;
uid_struct far *Head;
config_t far *Config;
flagdat_t far *Flagdat;

void already_running(void);
int setflag(int i, char far *ext);

void main(void)
{
	int i, handle;
	char buf[128];

	if (Running > 0)
		already_running();

	Running++;
	if ((Head = getuid_n("pwatch")) == 0)
		return;
	if ((Config = Head->us.p.config) == 0)
		return;
	if (Head->us.p.flagdat == 0) {
		if ((Head->us.p.flagdat = xmalloc(sizeof(flagdat_t) * 32)) == 0)
			return;

		memset(Head->us.p.flagdat, 0, sizeof(flagdat_t) * 32);
	}
	Flagdat = Head->us.p.flagdat;

	if ((handle = dopen(Config->dir.etcdir, "flag.txt", FA_READ)) == 0)
		return;
	for (i = 0; i < 32; i++) {
	redo:
		if (fgets(buf, sizeof(buf), handle) == 0)
			break;
		chop(buf);
		if (buf[0] == '#' || buf[0] == 0)
			goto redo;
		if (setflag(i, buf) != 0)
			goto redo;
	}
	close(handle);
}

void already_running(void)
{
	exit();
}

int setflag(int i, char far *ext)
{
	char far *tlimit_p, far *flagname;
	unsigned tlimit;

	if ((tlimit_p = strspl(ext, "\t ")) == 0)
		return -1;

	tlimit = atou(tlimit_p);
	if ((flagname = strspl(tlimit_p, "\t ")) == 0)
		return -2;

	if (strlen(ext) > sizeof(Flagdat[i].ext))
		return -3;
	if (strlen(flagname) > sizeof(Flagdat[i].flag_name))
		return -4;

	Flagdat[i].tlimit = tlimit;
	strcpy(Flagdat[i].ext, ext);
	strcpy(Flagdat[i].flag_name, flagname);

	return 0;
}

