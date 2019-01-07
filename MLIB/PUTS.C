
#include "klib.h"
#include "mlib.h"

void puts(char far *s)
{
	print(s);
	putchar('\n');
}

