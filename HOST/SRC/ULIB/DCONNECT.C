
#include "klib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void disconnect(void)
{
	char in[9], out[9];

	getstdio(getpid(), in, out);
	disconnect_device(in);
}

