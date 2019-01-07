
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

int save_boardlist(boardlist far *blist, char far *bname, config_t far *conf)
{
	int handle, w;
	long lastpos = 0;
	boardlist tmp;

	if ((handle = dopen(conf->dir.bbsdir, "blist.dat", FA_RW)) == 0)
		return 0;

	while (read(&tmp, sizeof(tmp), handle) != 0) {
		if (strcmp(tmp.realname, bname) == 0)
			break;

		lastpos = seek(handle, 0L, FS_CUR);
	}
	seek(handle, lastpos, FS_SET);
	w = write(blist, sizeof(*blist), handle);
	close(handle);

	return w;
}

