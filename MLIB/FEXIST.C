
#include "klib.h"

int file_exist(char far *filename)
{
	int handle;

	if ((handle = open(filename, FA_READ)) == 0)
		return 0;

	close(handle);
	return 1;
}

