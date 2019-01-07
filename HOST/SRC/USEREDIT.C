
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "bbsdat.h"
#include "cfgdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef void (*func_t)(userdat far *, uid_struct far *head);

char *wdaytab[] = { "Sun", "Mon", "The", "Wed", "Thu", "Fri", "Sat" };

void dispmenu(userdat far *uid);
void disptime(long t);
void dispflag(long flag);
uid_struct far *search_uidp(char far *id, uid_struct far *head);
void init_termdat(userdat far *udat, config_t far *conf, char far *id);
int make_homedir(char far *name, config_t far *conf);
int rename_homedir(char far *old, char far *new, config_t far *conf);
void edit_id(userdat far *udat, uid_struct far *head);
void edit_handle(userdat far *udat, uid_struct far *head);
void edit_password(userdat far *udat, uid_struct far *head);
void edit_flags(userdat far *udat, uid_struct far *head);
void edit_shell(userdat far *udat, uid_struct far *head);

func_t functab[] = {
	edit_id,
	edit_handle,
	edit_password,
	edit_flags,
	edit_shell,
};


void main(void)
{
	int newuser = 0;
	unsigned num;
	userdat udat, utmp;
	uid_struct far *uid, far *head, far *uch;
	char targetid[ID_MAX + 1], buf[2];

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	puts("<パスワードファイル修正>");

	print("ID : ");
	if (gets(targetid, sizeof(targetid)) == 0)
		return;
	if (search_usrdat(&udat, targetid, head->us.p.config) == 0) {
		puts("-- 新規ユーザー登録 --");
		memset(&udat, 0, sizeof(udat));
		init_termdat(&udat, head->us.p.config, targetid);
		newuser = 1;
	}
	putchar('\n');

	for (;;) {
		dispmenu(&udat);
		print("変更する項目番号 : ");
		if (gets(buf, 2) == 0) {
			if (cquery("終了しますか", Q_YES, uid) == Q_YES)
				break;
		}
		num = buf[0] - '0';
		if (num >= 1 && num <= ELEMS(functab))
			functab[num - 1](&udat, head);
	}

	if (cquery("書き込みますか", Q_NONE, uid) == Q_NO)
		puts("-- 破棄しました --");
	else {
		if ((uch = search_uidp(targetid, head)) != 0) {
			strcpy(uch->us.u.udat.id, udat.id);
			strcpy(uch->us.u.udat.password, udat.password);
			strcpy(uch->us.u.udat.handle, udat.handle);
			strcpy(uch->us.u.udat.shell, udat.shell);
		}
		if (newuser) {
			if (make_homedir(udat.id, head->us.p.config) != 0) {
				puts("** ホームディレクトリ作成に失敗しました **");
				goto error;
			}
		} else {
			if (rename_homedir(targetid, udat.id, head->us.p.config) != 0) {
				puts("** ホームディレクトリのリネームに失敗しました **");
				goto error;
			}
		}
		if (search_usrdat(&utmp, targetid, head->us.p.config) != 0) {
			strcpy(utmp.id, udat.id);
			strcpy(utmp.password, udat.password);
			strcpy(utmp.handle, udat.handle);
			strcpy(utmp.shell, udat.shell);
			utmp.usrflag = udat.usrflag;
		} else
			memcpy(&utmp, &udat, sizeof(udat));

		if (save_usrdat(&utmp, targetid, head->us.p.config) == 0)
			puts("** 書き込みに失敗しました **");
		
		puts("-- 保存しました --");
	}
error:
	;
}

void dispmenu(userdat far *udat)
{
	printf("Post         : %u\n", udat->post);
	printf("UPL/DOWNL    : %u/%u\n",
		udat->upload, udat->download);
	print("Last Login   : ");
		disptime(udat->last_login);
	print("Last Logout  : ");
		disptime(udat->last_logout);
	print("Last MsgRead : ");
		disptime(udat->last_msgread);

	putchar('\n');
	printf("[1] ID       : %s\n", udat->id);
	printf("[2] Password : %s\n", udat->password);
	printf("[3] Handle   : %s\n", udat->handle);
	print("[4] Flags    : ");
		dispflag(udat->usrflag);
	printf("[5] Shell    : %s\n\n", udat->shell);
}

void disptime(long t)
{
	time_struct lt;

	localtime(&lt, t);
	printf("%02d/%02d/%02d %02d:%02d:%02d (%s)\n",
		lt.year,lt.month,lt.day,lt.hour,lt.min,lt.sec, FAR(wdaytab[lt.week]));
}

void dispflag(long flag)
{
	int i, j;

	for (i = 0; i < 4 ; i++) {
		for (j = 0; j < 8; j++) {
			if (flag & 0x80000000L)
				putchar('o');
			else
				putchar('-');

			flag <<= 1;
		}
		if (i != 3)
			putchar(':');
	}
	putchar('\n');
}

uid_struct far *search_uidp(char far *id, uid_struct far *head)
{
	int i;

	for (i = 1; i <= head->us.p.maxusers; i++) {
		if (stricmp(head[i].us.u.udat.id, id) == 0
									&& head[i].chstat == CS_USING)
			return &head[i];
	}
	return 0;
}

int make_homedir(char far *name, config_t far *conf)
{
	char buf[128];

	sprintf(buf, "%s\\%s", FAR(conf->dir.homedir), name);
	return mkdir(buf);
}

int rename_homedir(char far *old, char far *new, config_t far *conf)
{
	char buf1[128], buf2[128];

	if (strcmp(old, new) == 0)
		return 0;

	sprintf(buf1, "%s\\%s", FAR(conf->dir.homedir), old);
	sprintf(buf2, "%s\\%s", FAR(conf->dir.homedir), new);
	return rename(buf1, buf2);
}

void init_termdat(userdat far *udat, config_t far *conf, char far *id)
{
	strcpy(udat->id, id);
	strcpy(udat->shell, conf->def_shell);
	strcpy(udat->term.prompt, conf->def_shprompt);
	udat->expert = 1;
	udat->term.term_width    = DEF_TERM_X;
	udat->term.term_height   = DEF_TERM_Y;
	udat->term.up_protocol   = PRTCL_XMODEM;
	udat->term.down_protocol = PRTCL_XMODEM;
	udat->term.editor_width  = DEF_EDITOR_F;
	udat->term.editor_lm     = DEF_EDITOR_M;
}

void edit_id(userdat far *udat, uid_struct far *head)
{
	char buf[ID_MAX + 1];
	userdat dummy;

	printf("ID : ");
	if (gets(buf, sizeof(buf)) != 0) {
		if (search_usrdat(&dummy, buf, head->us.p.config) != 0)
			puts("** その ID は既に存在します **\n");
		else
			strcpy(udat->id, buf);
	}
}

void edit_handle(userdat far *udat, uid_struct far *head)
{
	char buf[PASSWORD_MAX + 1];

	printf("Password : ");
	if (gets(buf, sizeof(buf)) != 0)
		strcpy(udat->password, buf);
}

void edit_password(userdat far *udat, uid_struct far *head)
{
	char buf[HANDLE_MAX + 1];

	printf("Handle : ");
	if (gets(buf, sizeof(buf)) != 0)
		strcpy(udat->handle, buf);
}

void edit_flags(userdat far *udat, uid_struct far *head)
{
	char argbuf[28];

	sprintf(argbuf, "%04X%04X ユーザーフラグ編集",
		FP_SEG(&udat->usrflag),FP_OFF(&udat->usrflag));
	spawn(P_WAIT, "flagedit.dlm", argbuf);
}

void edit_shell(userdat far *udat, uid_struct far *head)
{
	char buf[13];

	printf("Shell : ");
	if (gets(buf, sizeof(buf)) != 0)
		strcpy(udat->shell, buf);
}

