
#include "klib.h"

int getc(int handle)
{
	char c;

	if (read(&c, 1, handle) == 0)
		return -1;

	return c;
}

