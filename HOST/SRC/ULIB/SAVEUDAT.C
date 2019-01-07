
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

int save_usrdat(userdat far *usrdat, char far *id, config_t far *conf)
{
	int handle, w;
	long lastpos = 0;
	userdat udat;

	if ((handle = dopen(conf->dir.etcdir, "user.dat", FA_RW)) == 0)
		return 0;
	while (read(&udat, sizeof(udat), handle) != 0) {
		if (stricmp(udat.id, id) == 0)
			break;

		lastpos = seek(handle, 0L, FS_CUR);
	}
	seek(handle, lastpos, FS_SET);
	w = write(usrdat, sizeof(*usrdat), handle);
	close(handle);

	return w;
}

