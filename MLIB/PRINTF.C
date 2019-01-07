
#include "klib.h"
#include "mlib.h"

int printf(char far *format, ...)
{
	va_list ap;
	char buf[256];
	int vr;

	va_start(ap, format);
	vr = vsprintf(buf, format, ap);
	print(buf);

	return vr;
}

