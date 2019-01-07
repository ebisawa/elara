
#include "klib.h"
#include "../ioctl.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void disconnect_device(char far *device)
{
	unsigned lstat;

	lineoff(device);
	do {
		ioctl(device, IOCTL_GETLINESTAT, &lstat);
	} while (lstat & LSTAT_DCD);
	/*
	 * DTR は off にしたままにしておく
	 * 後で sendinit が初期化時に on にする
	 *
	 * こうしないと pwatch が回線切断を確認する前に着信がおきて
	 * ゾンビになる可能性がある
	 */
}

