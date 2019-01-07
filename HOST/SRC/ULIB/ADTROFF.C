
#include "klib.h"
#include "../config.h"
#include "../ioctl.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"

void alldtr_off(uid_struct far *head)
{
	int i;
	unsigned lstat;

	for (i = 1; i <= head->us.p.maxusers; i++) {
		ioctl(head[i].us.u.device_name, IOCTL_GETLINESTAT, &lstat);
		lstat &= ~LSTAT_DTR;
		ioctl(head[i].us.u.device_name, IOCTL_SETLINESTAT, &lstat);
	}
}

