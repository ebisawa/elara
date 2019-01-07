
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
int linecount(int handle, int far *aliasbytes);
unsigned long getflag(char far *p);

void main(void)
{
	int i = 0, handle, elems, aliasbytes;
	char far *p, far *q, far *ap, buf[256];
	void far *vp;
	config_t far *conf;
	uid_struct far *head;

	if (Running > 0)
		already_running();

	Running++;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((conf = head->us.p.config) == 0)
		return;
	if ((handle = dopen(conf->dir.etcdir, "command.txt", FA_READ)) == 0)
		return;

	elems = linecount(handle, &aliasbytes);
	head->us.p.maxcmd = 0;

	if ((vp = xmalloc(sizeof(command_t) * elems)) == 0)
		return;
	if (head->us.p.command != 0)
		xfree(head->us.p.command);

	head->us.p.command = vp;
	head->us.p.maxcmd = elems;
	memset(head->us.p.command, 0, sizeof(command_t) * elems);

	if ((vp = xmalloc(aliasbytes)) == 0)
		return;
	if (head->us.p.alias != 0)
		xfree(head->us.p.alias);

	head->us.p.alias = vp;
	ap = vp;

	while (fgets(buf, sizeof(buf), handle) != 0) {
		if (buf[0] == '#')
			continue;

		chop(buf);
		strlwr(buf);
		if ((p = strspl(buf, "\t ")) == 0)
			continue;
		if (strlen(buf) >= sizeof(head->us.p.command[i].name))
			continue;

		strcpy(head->us.p.command[i].name, buf);
		if ((q = strspl(p, "\t ")) == 0)
			head->us.p.command[i].alias = 0;
		else {
			strcpy(ap, q);
			head->us.p.command[i].alias = ap;
			ap += strlen(q) + 1;
		}
		head->us.p.command[i].flag = getflag(p);
		i++;
	}
	close(handle);
}

void already_running(void)
{
	exit();
}

int linecount(int handle, int far *aliasbytes)
{
	int count = 0;
	char far *p, buf[128];

	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);
		if (buf[0] != '#' && buf[0] != 0) {
			if ((p = strspl(buf, "\t ")) != 0) {
				if ((p = strspl(p, "\t ")) != 0)
					*aliasbytes += strlen(p) + 1;
			}
			count++;
		}
	}
	seek(handle, 0L, FS_SET);

	return count;
}

unsigned long getflag(char far *buf)
{
	unsigned long flag;

	for (flag = 0; *buf != 0; buf++) {
		if (*buf == ':')
			continue;

		flag <<= 1;
		if (*buf == 'o' || *buf == '1')
			flag |= 1;
	}
	return flag;
}

