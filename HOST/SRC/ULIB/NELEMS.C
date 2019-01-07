
#include "mlib.h"

int nelems(char far *p)
{
	int count = 0;

	while (*p != 0) {
		while (isspace(*p))
			*p++ = 0;
		if (!isspace(*p) && *p != 0) {
			count++;
			while (!isspace(*p) && *p != 0)
				p++;
		}
	}
	return count;
}

