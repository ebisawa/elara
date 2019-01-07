
#include "klib.h"
#include "mlib.h"

void dispdate(long t)
{
	time_struct ts;

	localtime(&ts, t);
	printf("%02d/%02d/%02d", ts.year, ts.month, ts.day);
}

