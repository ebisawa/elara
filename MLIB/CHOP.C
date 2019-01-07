
#include "mlib.h"

void chop(char far *s)
{
	int last = strlen(s) - 1;

	while (s[last] == '\r' || s[last] == '\n')
		s[last--] = 0;
}

