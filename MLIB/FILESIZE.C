
#include "klib.h"

long filesize(char far *filename)
{
	int handle;
	long pos;

	if ((handle = open(filename, FA_READ)) == 0)
		return 0;

	pos = seek(handle, 0L, FS_END);
	close(handle);

	return pos;
}

