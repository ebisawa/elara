
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void dispmenu(uid_struct far *head);
int getnum(void);

void main(void)
{
	int num;
	char far *argv = getargv();
	uid_struct far *uid, far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	if (argv == 0 || (num = atou(argv)) == 0) {
		dispmenu(head);
		print("EXPT)");
		if ((num = getnum()) == 0)
			return;
	}
	if (num < 0 || num > 3) {
		puts("** �s���Ȓl�ł� **");
		return;
	}
	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.expert = num;
	puts("-- �ύX���܂��� --");
}

void dispmenu(uid_struct far *head)
{
	puts("<�G�L�X�p�[�g���[�h�̐ݒ�>");
	typetext(head->us.p.config->dir.msgdir, "expert.msg");
}

int getnum(void)
{
	int i, n;
	char buf[2];

	buf[0] = 0;
	n = gets(buf, sizeof(buf));
	for (i = 0; i < n; i++) {
		if (!isdigit(buf[i]))
			return 0;
	}
	return atou(buf);
}
