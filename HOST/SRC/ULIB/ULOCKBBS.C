
#include "klib.h"
#include "mlib.h"
#include "../event.h"
#include "../config.h"
#include "../cfgdat.h"
#include "../bbsdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void unlockbbs(uid_struct far *uid, uid_struct far *head)
{
	head->us.p.bbslock = 0;
	raise(W_USER, EV_BBSLOCK);
	kill_lock(0);
}

