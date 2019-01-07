
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../ioctl.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void save_log_off(uid_struct far *uid, uid_struct far *head, long disconnect_time, int reason)
{
	int handle;
	long pos;
	accesslog log;

	if ((handle = dopen(head->us.p.config->dir.logdir,"log.dat", FA_RW)) == 0)
		return;

	pos = seek(handle, 0L, FS_END);
	for (;;) {
		pos -= sizeof(accesslog);
		seek(handle, pos, FS_SET);

		if (read(&log, sizeof(accesslog), handle) == 0) {
			close(handle);
			return;
		}
		if (log.count == uid->us.u.total_access) {
			if (log.connect_time == uid->us.u.connect_time)
				break;
		}
	}
	seek(handle, pos, FS_SET);

	log.disconnect_time = disconnect_time;
	log.disconnect_reason = reason;
	memcpy(&log.d_hist, &uid->us.u.dh, sizeof(log.d_hist));
	strcpy(log.handle, uid->us.u.udat.handle);

	write(&log, sizeof(accesslog), handle);
	close(handle);
}

