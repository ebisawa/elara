
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
		puts("<端末環境の設定>");
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

	print("現在の設定 : ");
	puts(uid->us.u.udat.term.prompt);
	print("新しい設定 : ");
	gets(buf, sizeof(buf));

	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	strcpy(uid->us.u.udat.term.prompt, buf);
	puts("-- 変更しました --");
}

void term_more(uid_struct far *uid)
{
	print("現在の設定 : ");
	print("more 制御");
	if (uid->us.u.udat.term.more == 1)
		puts("あり");
	else
		puts("なし");

	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.more = (uid->us.u.udat.term.more == 1) ? 0 : 1;
	printf("-- more 制御を行い%s --\n",
		(uid->us.u.udat.term.more == 1) ? FAR("ます") : FAR("ません"));
}

void term_esc(uid_struct far *uid)
{
	int num;

	puts("現在の設定");
	printf("カーソル制御 / 色制御 : [行%s] / [行%s]\n\n",
		(uid->us.u.udat.term.esc & ESC_CURSOR) ? FAR("う") : FAR("わない"),
		(uid->us.u.udat.term.esc & ESC_COLOR)  ? FAR("う") : FAR("わない"));

	puts("[1] カーソル制御なし/色制御なし (dumb)");		/* 0 */
	puts("[2] カーソル制御あり/色制御なし");			/* 1 */
	puts("[3] カーソル制御なし/色制御あり");			/* 2 */
	puts("[4] カーソル制御あり/色制御あり");			/* 3 */
	print("ESC)");

	if ((num = getnum()) == -1)
		return;
	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	num--;
	uid->us.u.udat.term.esc = 0;
	uid->us.u.udat.term.esc |= (num & 1) ? ESC_CURSOR : 0;
	uid->us.u.udat.term.esc |= (num & 2) ? ESC_COLOR  : 0;

	puts("-- 変更しました --");
}

void term_protocol(uid_struct far *uid)
{
	int up, down;

	puts("ファイル転送プロトコルの変更");
	up = prtcl_upload(uid);
	down = prtcl_download(uid);

	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	if (up != 0)
		uid->us.u.udat.term.up_protocol = prtcl_tab_up[up - 1];
	if (down != 0)
		uid->us.u.udat.term.down_protocol = prtcl_tab_down[down - 1];

	puts("-- 変更しました --");
}

void term_editf(uid_struct far *uid)
{
	int num;

	printf("現在の%s設定 : %d\n",
		FAR(" /F: "), uid->us.u.udat.term.editor_width);
	print("新しい設定値 : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.editor_width = num;
	puts("-- 変更しました --");
}

void term_editm(uid_struct far *uid)
{
	int num;

	printf("現在の%s設定 : %d\n",
		FAR(" /M: "), uid->us.u.udat.term.editor_lm);
	print("新しい設定値 : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.editor_lm = num;
	puts("-- 変更しました --");
}

void term_width(uid_struct far *uid)
{
	int num;

	printf("現在の%s設定 : %d\n",
		FAR("桁数(X)"), uid->us.u.udat.term.term_width);
	print("新しい設定値 : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.term_width = num;
	puts("-- 変更しました --");
}

void term_height(uid_struct far *uid)
{
	int num;

	printf("現在の%s設定 : %d\n",
		FAR("行数(Y)"), uid->us.u.udat.term.term_height);
	print("新しい設定値 : ");

	if ((num = getnum()) == -1)
		return;
	if (cquery("変更しますか", Q_NONE, uid) != Q_YES)
		return;

	uid->us.u.udat.term.term_height = num;
	puts("-- 変更しました --");
}

int prtcl_upload(uid_struct far *uid)
{
	int num;

	puts("アップロードに使うプロトコル");
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

	puts("ダウンロードに使うプロトコル");
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

