
#include "mlib.h"

void strcat(char far *s, char far *add)
{
	while (*s != 0)
		s++;
	strcpy(s, add);
}

