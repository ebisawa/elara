
#include "klib.h"
#include "../ioctl.h"

void lineon(char far *device_name)
{
	unsigned lstat;

	ioctl(device_name, IOCTL_GETLINESTAT, &lstat);
	lstat |= LSTAT_DTR;
	ioctl(device_name, IOCTL_SETLINESTAT, &lstat);
}

