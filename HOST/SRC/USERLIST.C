
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

unsigned long getnusers(int handle);

void main(void)
{
	int handle;
	char buf[11];
	unsigned long nusers, pos = 0;
	uid_struct far *head;
	userdat udat;

	if ((head = getuid_n("pwatch")) == 0)
		return;

	handle = dopen(head->us.p.config->dir.etcdir, "user.dat", FA_READ);
	if (handle == 0)
		return;

	nusers = getnusers(handle);
	printf("何番目のユーザーから表示しますか ? (max.%ld) CR=先頭:q=脱出 : ",
		nusers - 1);

	nusers = 0;
	if (gets(buf, sizeof(buf)) != 0) {
		if ((nusers = atol(buf)) == 0)
			goto end;

		pos = lmul(sizeof(userdat), nusers);
	}
	seek(handle, pos, FS_SET);

	while (read(&udat, sizeof(udat), handle) != 0) {
		if (getchar() != -1)
			break;

		printf("(%ld) %-8s  [%s]\n", nusers, FAR(udat.id), FAR(udat.handle));
		nusers++;
	}

end:
	close(handle);
}

unsigned long getnusers(int handle)
{
	ldiv_t d;
	unsigned long pos;

	pos = seek(handle, 0L, FS_END);
	d = ldiv(pos, sizeof(userdat));
	seek(handle, 0L, FS_SET);

	return d.quot;
}

