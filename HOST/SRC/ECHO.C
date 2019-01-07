
#include "klib.h"
#include "mlib.h"

void main(void)
{
	char far *argv;

	if ((argv = getargv()) == 0)
		putchar('\n');
	else
		puts(argv);
}

