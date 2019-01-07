
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void initialize(char far *device_name, uid_struct far *head);
uid_struct far *search_uid_d(char far *device_name, uid_struct far *head);
int ispoweron(char far *device_name);

void main(void)
{
	int i, handle;
	char buf[256];
	uid_struct far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((handle = open("portinit", FA_READ)) == 0)
		return;

	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);
		if (buf[0] == '#' || buf[0] == 0)
			continue;

		initialize(buf, head);
	}
	close(handle);

	for (i = 1; i <= head->us.p.maxusers; i++) {
		if (ispoweron(head[i].us.u.device_name))
			spawn(P_NOWAIT, "sendinit.dlm", head[i].us.u.device_name);
	}
}

void initialize(char far *device_name, uid_struct far *head)
{
	char far *speed_p, far *initstr;
	long speed;
	uid_struct far *uid;

	if ((speed_p = strspl(device_name, "\t ")) == 0)
		return;
	if ((uid = getuid_d(device_name, head)) == 0)
		return;

	initstr = strspl(speed_p, "\t ");
	speed = atol(speed_p);
	ioctl(device_name, IOCTL_SETSPEED, &speed);

	if (initstr != 0) {
		if (strlen(initstr) > sizeof(uid->us.u.initstr))
			return;

		strcpy(uid->us.u.initstr, initstr);
	}
}

int ispoweron(char far *device_name)
{
	unsigned lstat;

	ioctl(device_name, IOCTL_GETLINESTAT, &lstat);

	return lstat & LSTAT_DSR;
}

