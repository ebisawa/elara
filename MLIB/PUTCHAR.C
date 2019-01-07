
#include "klib.h"

static void xputchar(char c);

int putchar(char c)
{
	if (c != '\n')
		xputchar(c);
	else {
		xputchar('\r');
		xputchar('\n');
	}
	return 0;
}

static void xputchar(char c)
{
	while (write(&c, 1, 0) == 0)
		relcpu();
}

