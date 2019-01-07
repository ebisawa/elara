
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	unsigned pid = getpid();
	uid_struct far *uid, far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(pid)) == 0)
		return;

	uid->us.u.logoff = DR_LOG_OFF;
	if (uid->us.u.open_flag)
		uid->us.u.udat.last_msgread = uid->us.u.login_time;

	typetext(head->us.p.config->dir.msgdir, "logout.msg");

	waitsec(1, head);
	disconnect();

	for (;;)
		relcpu();	/* pwatch ‚ª‰ñüØ’f‚ğŒŸo‚·‚é‚Ü‚Å‘Ò‚Â */
}

