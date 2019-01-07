
#include "klib.h"
#include "mlib.h"
#include "../config.h"

int dopen2(char far *dir1, char far *dir2, char far *name, unsigned mode)
{
	char buf[128];

	sprintf(buf,"%s\\%s\\%s", FAR(dir1), FAR(dir2), FAR(name));
	return open(buf, mode);
}

