
#include "klib.h"
#include "mlib.h"

int sprintf(char far *buf, char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	return vsprintf(buf, format, ap);
}

