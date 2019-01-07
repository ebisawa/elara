
#include "mlib.h"

static int isdelim(int c, char far *delim);
static char far *whiledelim(char far *s, char far *delim);

char far *strspl(char far *s, char far *delim)
{
	if ((s = whiledelim(s, delim)) == 0)
		return 0;
	while (!isdelim(*s, delim) && *s != 0)
		s++;

	if (*s != 0) {
		*s = 0;
		return whiledelim(s + 1, delim);
	}
	return 0;
}

static int isdelim(int c, char far *delim)
{
	while (*delim != 0) {
		if (c == *delim)
			return 1;

		delim++;
	}
	return 0;
}

static char far *whiledelim(char far *s, char far *delim)
{
	while (isdelim(*s, delim) && *s != 0)
		s++;
	if (*s == 0)
		return 0;

	return s;
}

