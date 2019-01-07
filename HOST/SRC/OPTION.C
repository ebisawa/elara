
#include "klib.h"
#include "mlib.h"

void main(void)
{
	char far *argv = getargv();

	printf("option: [%s]\n", (argv == 0) ? "" : argv);
}

