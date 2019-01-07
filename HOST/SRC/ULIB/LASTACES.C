
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../ioctl.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void last_access(accesslog far *log, config_t far *conf)
{
	int handle;
	long pos;

	memset(log, 0, sizeof(accesslog));
	if ((handle = dopen(conf->dir.logdir, "log.dat", FA_READ)) == 0)
		return;

	pos = seek(handle, 0L, FS_END);
	pos -= sizeof(accesslog);
	seek(handle, pos, FS_SET);
	read(log, sizeof(accesslog), handle);
	close(handle);
}

