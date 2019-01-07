
#include "klib.h"

void filecopy_n(char far *src, char far *dst)
{
	char buf[256];
	int len, handle_src, handle_dst;

	if ((handle_src = open(src, FA_READ)) == 0)
		return;
	if ((handle_dst = open(dst, FA_APPEND)) == 0)
		return;

	for (;;) {
		len = read(buf, sizeof(buf), handle_src);
		write(buf, len, handle_dst);
		if (len < sizeof(buf))
			break;
	}
}

