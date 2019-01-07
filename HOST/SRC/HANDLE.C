
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
		puts("<�n���h���l�[���̕ύX>");
		printf("���݂̃n���h���l�[�� : %s\n", uid->us.u.udat.handle);
		print("�V�����n���h���l�[�� : ");
		gets(buf, sizeof(buf));
		argv = buf;

		if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
			return;
	}
	if (strlen(argv) > HANDLE_MAX)
		argv[HANDLE_MAX] = 0;

	strcpy(uid->us.u.udat.handle, argv);
	printf("-- %s �ɕύX���܂��� --\n", argv);
}
