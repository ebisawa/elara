
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../ioctl.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void save_log_preset(uid_struct far *uid, uid_struct far *head, int count)
{
	char far *result;
	accesslog log;

	memset(&log, 0, sizeof(log));
	log.count = count;
	strcpy(log.id, uid->us.u.udat.id);
	strcpy(log.handle, uid->us.u.udat.handle);
	ioctl(uid->us.u.device_name, IOCTL_GETRESULT, &result);
	strcpy(log.result, result);
	log.connect_time = uid->us.u.connect_time;
	log.disconnect_time = 0;
	log.channel = getchnum(uid->us.u.device_name, head);
	log.disconnect_reason = 0;

	savelog_file(&log, head->us.p.config);
}

