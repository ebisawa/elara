
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../ioctl.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void save_log_nouser(uid_struct far *uid, uid_struct far *head, long connect_time, long disconnect_time, int reason)
{
	accesslog log, last;

	memset(&log, 0, sizeof(log));
	last_access(&last, head->us.p.config);
	log.count = last.count;
	log.connect_time = connect_time;
	log.disconnect_time = disconnect_time;
	log.disconnect_reason = reason;
	log.channel = getchnum(uid->us.u.device_name, head);

	savelog_file(&log, head->us.p.config);
}

