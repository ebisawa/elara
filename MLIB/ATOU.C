
#include "mlib.h"

unsigned atou(char far *s)
{
	unsigned n = 0;

	while (isdigit(*s)) {
		n *= 10;
		n += *s - '0';
		s++;
	}
	return n;
}

