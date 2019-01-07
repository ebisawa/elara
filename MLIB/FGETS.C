
#include "klib.h"
#include "mlib.h"

#if 1

int fgets(char far *buf, int max, int handle)
{
	int i, len;
	long pos;

	pos = seek(handle, 0L, FS_CUR);
	len = read(buf, max - 1, handle);
	for (i = 0; i < len; i++) {
		if (buf[i] == '\n') {
			i++;
			break;
		}
	}
	buf[i] = 0;
	seek(handle, pos + i, FS_SET);
	return i;
}

#else

int fgets(char far *buf, int max, int handle)
{
	int i, c;

	for (i = 0; i < max - 1; i++) {
		if ((c = getc(handle)) == -1)
			break;

		buf[i] = c;
		if (c == '\n') {
			i++;
			break;
		}
	}
	buf[i] = 0;
	return i;
}


#endif


