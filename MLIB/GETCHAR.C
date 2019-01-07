
#include "klib.h"

int __ungetch_buf = -1;

int getchar(void)
{
	char c;

	if (__ungetch_buf != -1) {
		c = __ungetch_buf;
		__ungetch_buf = -1;
		return c;
	}

again:
	if (read(&c, 1, 0) == 0)
		return -1;
	if (c == '\n')
		goto again;
	if (c == '\r')		/* CR, CR + LF ‚Í LF ‚É’u‚«Š·‚¦‚é */
		c = '\n';

	return c;
}

