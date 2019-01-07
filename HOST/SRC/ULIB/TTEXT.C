
#include "klib.h"
#include "mlib.h"
#include "../config.h"

int typetext(char far *dir, char far *filename)
{
	char buf[128];

	sprintf(buf, "%s\\%s", (char far *)dir, (char far *)filename);
	return spawn(P_WAIT, "type.dlm", buf);

}

