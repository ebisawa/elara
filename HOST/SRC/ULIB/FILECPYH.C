
#include "klib.h"

void filecopy_h(int src, int dst)
{
	int len;
	char buf[256];

	seek(src, 0L, FS_SET);
	seek(dst, 0L, FS_END);
	for (;;) {
		len = read(buf, sizeof(buf), src);
		write(buf, len, dst);
		if (len < sizeof(buf))
			break;
	}
}

