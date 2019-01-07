
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	char far *argv, buf[6];
	unsigned target;

	if ((argv = getargv()) == 0) {
		print("pid : ");
		if (gets(buf, sizeof(buf)) == 0)
			return;
		argv = buf;
	}
	target = atou(argv);
	if (kill(target) != 0)
		puts("** é∏îsÇµÇ‹ÇµÇΩ **");
	else
		puts("-- ã≠êßèIóπÇµÇ‹ÇµÇΩ --");
}

