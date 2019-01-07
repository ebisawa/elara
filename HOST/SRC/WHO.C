
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
	int i;
	char far *p, handle[HANDLE_MAX + 2 + 1];
	uid_struct far *head, far *uid;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	if (head != 0) {
		putchar('\n');
		for (i = 1; i <= head->us.p.maxusers; i++) {
			ioctl(head[i].us.u.device_name, IOCTL_GETRESULT, &p);
			printf("Ch%-2d", i);
			if (head[i].chstat == CS_USING) {
				sprintf(handle, "(%s)", head[i].us.u.udat.handle);
				printf(" %c %-8s %-18s  [%-8s] %s",
					(&head[i] == uid) ? '*' : ' ',
					FAR(head[i].us.u.udat.id),
					FAR(handle),
					FAR(head[i].us.u.ucmd),
					FAR(p));
			}
			putchar('\n');
		}
	}
}

