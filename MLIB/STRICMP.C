
#include "mlib.h"

int stricmp(char far *a, char far *b)
{
	int c1, c2;

	while (*a != 0 && *b != 0) {
		c1 = toupper(*a);
		c2 = toupper(*b);
		if (c1 != c2)
			break;

		a++;
		b++;
	}
	return c1 - c2;
}

