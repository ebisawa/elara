
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"

int getchnum(char far *device_name, uid_struct far *head)
{
	int i;

	for (i = 1; i <= head->us.p.maxusers; i++) {
		if (strcmp(head[i].us.u.device_name, device_name) == 0)
			return i;
	}
	return 0;
}

