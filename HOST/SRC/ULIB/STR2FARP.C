
#include "mlib.h"

void far *str2farp(char far *str)
{
	int i;
	long p = 0;

	for (i = 0; i < 8; i++) {
		p = lmul(p, 16);
		if (isdigit(*str))
			p += (*str - '0');
		else if (isupper(*str))
			p += (*str - 'A') + 10;
		else
			return 0;

		str++;
	}

	return (long far *)p;
}

