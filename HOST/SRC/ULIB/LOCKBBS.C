
#include "klib.h"
#include "mlib.h"
#include "../event.h"
#include "../config.h"
#include "../cfgdat.h"
#include "../bbsdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void lockbbs(uid_struct far *uid, uid_struct far *head)
{
again:
	if (head->us.p.bbslock != 0) {
		puts("[WAIT]");
		wait(W_USER, EV_BBSLOCK);
		goto again;
	}
	kill_lock(1);
	head->us.p.bbslock = getchnum(uid->us.u.device_name, head);
}

