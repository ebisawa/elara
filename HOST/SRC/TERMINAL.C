
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef void (*func_t)(uid_struct far *);

int option(char far *argv);
void dispmenu(uid_struct far *head);
int getnum(void);
void term_prompt(uid_struct far *uid);
void term_more(uid_struct far *uid);
void term_esc(uid_struct far *uid);
void term_protocol(uid_struct far *uid);
void term_editf(uid_struct far *uid);
void term_editm(uid_struct far *uid);
void term_width(uid_struct far *uid);
void term_height(uid_struct far *uid);
int prtcl_upload(uid_struct far *uid);
int prtcl_download(uid_struct far *uid);

func_t ftab[] = {
	term_prompt,
	term_more,
	term_esc,
	term_protocol,
	term_editf,
	term_editm,
	term_width,
	term_height,
};

char optab[] = { 'c', 't', 'e', 'p', 'f', 'm', 'x', 'y' };

char prtcl_tab_up[] = {
	PRTCL_ASCII, PRTCL_XMODEM, PRTCL_YMODEM, PRTCL_YMODEM_G
};

char *prtcl_name_up[] = {
	"ASCII", "XMODEM (128/SUM)", "YMODEM", "YMODEM-g"
};

char prtcl_tab_down[] = {
	PRTCL_ASCII, PRTCL_XMODEM, PRTCL_YMODEM_BATCH
};

char *prtcl_name_down[] = {
	"ASCII", "XMODEM (128/SUM)", "YMODEM (Batch)"
};


void main(void)
{
	int num;
	char far *argv = getargv();
	uid_struct far *uid, far *head;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	if ((num = option(argv)) != -1)
		ftab[num](uid);
	else {
		puts("<�[�����̐ݒ�>");
		for (;;) {
			dispmenu(head);
			print("TERM)");

			if ((num = getnum()) == -1)
				return;
			if (num < 1 || num > 8)
				return;

			num--;
			ftab[num](uid);
		}
	}
}

int option(char far *argv)
{
	int i;

	if (argv == 0)
		return -1;
	while (*argv != '-') {
		if (*argv == 0)
			return -1;

		argv++;
	}
	if (*++argv == 0)
		return -1;
	*argv = tolower(*argv);

	for (i = 0; i < ELEMS(optab); i++) {
		if (optab[i] == *argv)
			return i;
	}
	return -1;
}

void dispmenu(uid_struct far *head)
{
	typetext(head->us.p.config->dir.msgdir, "terminal.msg");
}

int getnum(void)
{
	int i, n;
	char buf[4];

	buf[0] = 0;
	n = gets(buf, sizeof(buf));
	for (i = 0; i < n; i++) {
		if (!isdigit(buf[i]))
			return -1;
	}
	return atou(buf);
}

void term_prompt(uid_struct far *uid)
{
	char buf[PROMPT_MAX + 1];

	print("���݂̐ݒ� : ");
	puts(uid->us.u.udat.term.prompt);
	print("�V�����ݒ� : ");
	gets(buf, sizeof(buf));

	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	strcpy(uid->us.u.udat.term.prompt, buf);
	puts("-- �ύX���܂��� --");
}

void term_more(uid_struct far *uid)
{
	print("���݂̐ݒ� : ");
	print("more ����");
	if (uid->us.u.udat.term.more == 1)
		puts("����");
	else
		puts("�Ȃ�");

	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.more = (uid->us.u.udat.term.more == 1) ? 0 : 1;
	printf("-- more ������s��%s --\n",
		(uid->us.u.udat.term.more == 1) ? FAR("�܂�") : FAR("�܂���"));
}

void term_esc(uid_struct far *uid)
{
	int num;

	puts("���݂̐ݒ�");
	printf("�J�[�\������ / �F���� : [�s%s] / [�s%s]\n\n",
		(uid->us.u.udat.term.esc & ESC_CURSOR) ? FAR("��") : FAR("��Ȃ�"),
		(uid->us.u.udat.term.esc & ESC_COLOR)  ? FAR("��") : FAR("��Ȃ�"));

	puts("[1] �J�[�\������Ȃ�/�F����Ȃ� (dumb)");		/* 0 */
	puts("[2] �J�[�\�����䂠��/�F����Ȃ�");			/* 1 */
	puts("[3] �J�[�\������Ȃ�/�F���䂠��");			/* 2 */
	puts("[4] �J�[�\�����䂠��/�F���䂠��");			/* 3 */
	print("ESC)");

	if ((num = getnum()) == -1)
		return;
	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	num--;
	uid->us.u.udat.term.esc = 0;
	uid->us.u.udat.term.esc |= (num & 1) ? ESC_CURSOR : 0;
	uid->us.u.udat.term.esc |= (num & 2) ? ESC_COLOR  : 0;

	puts("-- �ύX���܂��� --");
}

void term_protocol(uid_struct far *uid)
{
	int up, down;

	puts("�t�@�C���]���v���g�R���̕ύX");
	up = prtcl_upload(uid);
	down = prtcl_download(uid);

	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	if (up != 0)
		uid->us.u.udat.term.up_protocol = prtcl_tab_up[up - 1];
	if (down != 0)
		uid->us.u.udat.term.down_protocol = prtcl_tab_down[down - 1];

	puts("-- �ύX���܂��� --");
}

void term_editf(uid_struct far *uid)
{
	int num;

	printf("���݂�%s�ݒ� : %d\n",
		FAR(" /F: "), uid->us.u.udat.term.editor_width);
	print("�V�����ݒ�l : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.editor_width = num;
	puts("-- �ύX���܂��� --");
}

void term_editm(uid_struct far *uid)
{
	int num;

	printf("���݂�%s�ݒ� : %d\n",
		FAR(" /M: "), uid->us.u.udat.term.editor_lm);
	print("�V�����ݒ�l : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.editor_lm = num;
	puts("-- �ύX���܂��� --");
}

void term_width(uid_struct far *uid)
{
	int num;

	printf("���݂�%s�ݒ� : %d\n",
		FAR("����(X)"), uid->us.u.udat.term.term_width);
	print("�V�����ݒ�l : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.term_width = num;
	puts("-- �ύX���܂��� --");
}

void term_height(uid_struct far *uid)
{
	int num;

	printf("���݂�%s�ݒ� : %d\n",
		FAR("�s��(Y)"), uid->us.u.udat.term.term_height);
	print("�V�����ݒ�l : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("�ύX���܂���", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.term_height = num;
	puts("-- �ύX���܂��� --");
}

int prtcl_upload(uid_struct far *uid)
{
	int num;

	puts("�A�b�v���[�h�Ɏg���v���g�R��");
	for (num = 0; num < 4; num++) {
		printf("[%d]%c%s\n", num + 1,
			(uid->us.u.udat.term.up_protocol == prtcl_tab_up[num]) ? '*':' ',
			FAR(prtcl_name_up[num]));
	}
	print("PRTCL)");

	if ((num = getnum()) == -1)
		return 0;
	if (num < 1 || num > 4)
		return 0;

	return num;
}

int prtcl_download(uid_struct far *uid)
{
	int num;

	puts("�_�E�����[�h�Ɏg���v���g�R��");
	for (num = 0; num < 3; num++) {
		printf("[%d]%c%s\n", num + 1,
			(uid->us.u.udat.term.down_protocol == prtcl_tab_down[num])?'*':' ',
			FAR(prtcl_name_down[num]));
	}
	print("PRTCL)");

	if ((num = getnum()) == -1)
		return 0;
	if (num < 1 || num > 3)
		return 0;

	return num;
}
