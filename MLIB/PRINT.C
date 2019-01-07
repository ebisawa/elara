
#include "klib.h"
#include "mlib.h"

void print(char far *s)
{
	while (*s != 0)
		putchar(*s++);
}

