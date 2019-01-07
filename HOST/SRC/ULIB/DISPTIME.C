
#include "klib.h"
#include "mlib.h"

void disptime(long t)
{
	time_struct ts;

	localtime(&ts, t);
	printf("%02d:%02d:%02d", ts.hour, ts.min, ts.sec);
}

