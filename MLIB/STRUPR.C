
#include "mlib.h"

void strupr(char far *s)
{
	while (*s != 0) {
		*s = toupper(*s);
		s++;
	}
}

