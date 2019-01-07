
#include "mlib.h"

void strlwr(char far *s)
{
	while (*s != 0) {
		*s = tolower(*s);
		s++;
	}
}

