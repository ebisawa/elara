
#include "mlib.h"

char far *strstr(char far *a, char far *b)
{
	char far *p;

	p = a;
	while ((p = strchr(p, *b)) != 0) {
		if (strncmp(p, b, strlen(b)) == 0)
			return p;

		p++;
	}
	return 0;
}

