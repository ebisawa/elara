
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

int search_board(boardlist far *blist, char far *bname, config_t far *conf)
{
	int handle, found = 0;

	if ((handle = dopen(conf->dir.bbsdir, "blist.dat", FA_READ)) == 0)
		return 0;
	while (read(blist, sizeof(*blist), handle) != 0) {
		if (strcmp(blist->realname, bname) == 0) {
			found++;
			break;
		}
	}
	close(handle);
	return found;
}

