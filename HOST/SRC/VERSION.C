
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "version.h"

#define MSTR(s)		MSTR2(s)
#define MSTR2(s)	# s

char VersionString[] = \
		 MSTR(ELARA_VERMAJ) "." MSTR(ELARA_VERMIN) "." MSTR(ELARA_BUILD) \
		 ELARA_VERALPHA;

void main(void)
{
	char stdin[9], stdout[9];
	char far *argv;

	argv = getargv();
	getstdio(getpid(), stdin, stdout);
	if (stdin[0] == 0)
		disp(VersionString);
	else {
		if (argv != 0 && strcmp(argv, "-numberonly") == 0)
			print(VersionString);
		else {
			printf("Elara version %s\n", FAR(VersionString));
			puts("Copyright (C) S.Ebisawa, 1995. All rights reserved.");
		}
	}
}

