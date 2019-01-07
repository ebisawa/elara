
#include "klib.h"
#include "mlib.h"
#include "../config.h"

int dopen(char far *dir, char far *filename, unsigned mode)
{
	char buf[128];

	sprintf(buf, "%s\\%s", (char far *)dir, (char far *)filename);
	return open(buf, mode);
}

