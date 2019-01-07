
#include "klib.h"
#include "mlib.h"

int gets2(char far *buf, int max, int echoback)
{
	return gets3(buf, max, echoback, 0);
}

