
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

	puts("<�p�X���[�h�̕ύX>");
	print("���݂̃p�X���[�h����͂��Ă������� : ");
	gets2(buf, sizeof(buf), '#');
	if (strcmp(uid->us.u.udat.password, buf) == 0) {
		print("�V�����p�X���[�h����͂��Ă������� : ");
		if (gets2(buf, sizeof(buf), '#') != 0) {
			print("������x�V�����p�X���[�h����͂��Ă������� : ");
			gets2(buf2, sizeof(buf2), '?');
			if (strcmp(buf, buf2) == 0) {
				if (cquery("�ύX���܂���", Q_NONE, uid) == Q_YES) {
					strcpy(uid->us.u.udat.password, buf);

					/* �p�X���[�h�͏d�v�Ȃ̂ő����ɃZ�[�u */
					save_usrdat(&uid->us.u.udat, uid->us.u.udat.id,
						head->us.p.config);
					puts("-- �ύX���܂��� --");
					return;
				}
			}
		}
	}
	puts("** �ύX�𒆎~���܂� **");
}
