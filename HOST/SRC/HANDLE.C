
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	char far *argv, buf[HANDLE_MAX + 1];
	uid_struct far *uid;

	if ((uid = getuid(getpid())) == 0)
		return;
	if ((argv = getargv()) == 0) {
		puts("<ハンドルネームの変更>");
		printf("現在のハンドルネーム : %s\n", uid->us.u.udat.handle);
		print("新しいハンドルネーム : ");
		gets(buf, sizeof(buf));
		argv = buf;

		if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
			return;
	}
	if (strlen(argv) > HANDLE_MAX)
		argv[HANDLE_MAX] = 0;

	strcpy(uid->us.u.udat.handle, argv);
	printf("-- %s に変更しました --\n", argv);
}

