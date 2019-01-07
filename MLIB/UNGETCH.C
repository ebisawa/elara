
#include "klib.h"

extern int __ungetch_buf;

void ungetch(int c)
{
	__ungetch_buf = c;
}

