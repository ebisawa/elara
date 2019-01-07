
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	char buf[PASSWORD_MAX + 1], buf2[PASSWORD_MAX + 1];
	uid_struct far *uid, far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	puts("<パスワードの変更>");
	print("現在のパスワードを入力してください : ");
	gets2(buf, sizeof(buf), '#');
	if (strcmp(uid->us.u.udat.password, buf) == 0) {
		print("新しいパスワードを入力してください : ");
		if (gets2(buf, sizeof(buf), '#') != 0) {
			print("もう一度新しいパスワードを入力してください : ");
			gets2(buf2, sizeof(buf2), '?');
			if (strcmp(buf, buf2) == 0) {
				if (cquery("変更しますか", Q_NONE, uid) == Q_YES) {
					strcpy(uid->us.u.udat.password, buf);

					/* パスワードは重要なので即座にセーブ */
					save_usrdat(&uid->us.u.udat, uid->us.u.udat.id,
						head->us.p.config);
					puts("-- 変更しました --");
					return;
				}
			}
		}
	}
	puts("** 変更を中止します **");
}

