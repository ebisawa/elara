
#include "mlib.h"

long atol2(char far *s, int radix)
{
	long n = 0;

	while (isdigit(*s)) {
		n = lmul(n, radix);
		n += *s - '0';
		s++;
	}
	return n;
}

