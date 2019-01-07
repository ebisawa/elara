
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
int linecount(int handle);
unsigned long getflag(char far *p);

void main(void)
{
	int i = 0, handle, elems;
	char far *p, buf[256];
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
	if ((handle = dopen(conf->dir.etcdir, "option.txt", FA_READ)) == 0)
		return;

	elems = linecount(handle);
	head->us.p.maxopt = 0;

	if ((vp = xmalloc(sizeof(option_t) * elems)) == 0)
		return;
	if (head->us.p.option != 0)
		xfree(head->us.p.option);

	head->us.p.option = vp;
	head->us.p.maxopt = elems;
	memset(head->us.p.option, 0, sizeof(option_t) * elems);

	while (fgets(buf, sizeof(buf), handle) != 0) {
		if (buf[0] == '#')
			continue;

		chop(buf);
		strlwr(buf);
		if ((p = strspl(buf, "\t ")) == 0)
			continue;
		if (strlen(buf) >= sizeof(head->us.p.option[i].name))
			continue;

		strcpy(head->us.p.option[i].name, buf);
		head->us.p.option[i].flag = getflag(p);
		i++;
	}
	close(handle);
}

void already_running(void)
{
	exit();
}

int linecount(int handle)
{
	int count = 0;
	char buf[128];

	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);
		if (buf[0] != '#' && buf[0] != 0)
			count++;
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

