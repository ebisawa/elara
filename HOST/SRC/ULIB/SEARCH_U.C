
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

int search_usrdat(userdat far *usrdat, char far *id, config_t far *conf)
{
	int handle;

	if ((handle = dopen(conf->dir.etcdir, "user.dat", FA_READ)) == 0)
		return 0;
	while (read(usrdat, sizeof(userdat), handle) != 0) {
		if (stricmp(usrdat->id, id) == 0) {
			close(handle);
			return 1;
		}
	}
	close(handle);

	return 0;
}

