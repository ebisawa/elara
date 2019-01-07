
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	char far *p = getargv();

	if (p == 0)
		p = "";

	system(p);
}

