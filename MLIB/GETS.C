
#include "mlib.h"

int gets(char far *buf, int max)
{
	return gets2(buf, max, 0);
}

