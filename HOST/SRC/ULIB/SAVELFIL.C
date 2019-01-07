
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../ioctl.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void savelog_file(accesslog far *log, config_t far *conf)
{
	int handle;

	if ((handle = dopen(conf->dir.logdir, "log.dat", FA_APPEND)) == 0)
		return;

	write(log, sizeof(accesslog), handle);
	close(handle);
}

