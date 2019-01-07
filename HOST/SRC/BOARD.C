
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef void (*func_t)(boardlist far *, uid_struct far *head);

char *wdaytab[] = { "Sun", "Mon", "The", "Wed", "Thu", "Fri", "Sat" };

void dispmenu(boardlist far *blist);
void disptime(long t);
void dispflag(long flag);
void edit_boardname(boardlist far *blist, uid_struct far *head);
void edit_boardtitle(boardlist far *blist, uid_struct far *head);
void edit_dosname(boardlist far *blist, uid_struct far *head);
void edit_read(boardlist far *blist, uid_struct far *head);
void edit_write(boardlist far *blist, uid_struct far *head);
void edit_basenote(boardlist far *blist, uid_struct far *head);
int make_bdir(char far *dosname, config_t far *conf);
int check_dosname(char far *dosname, config_t far *conf);
int rename_bdir(char far *old, char far *new, config_t far *conf);

func_t functab[] = {
	edit_boardname,
	edit_boardtitle,
	edit_dosname,
	edit_read,
	edit_write,
	edit_basenote,
};

void main(void)
{
	int i, num, newboard = 0;
	char far *argv, buf[2], target[BOARDNAME_MAX + 1];
	char old_dosname[9];
	boardlist blist, btmp;
	uid_struct far *head, far *uid;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	if ((argv = getargv()) == 0) {
		print("boardname : ");
		if (gets(target, sizeof(target)) == 0)
			return;
	} else {
		if (strlen(argv) > sizeof(target) - 1)
			return;

		strcpy(target, argv);
	}

	if (search_board(&blist, target, head->us.p.config) == 0) {
		puts("-- new board --");
		newboard = 1;
		memset(&blist, 0, sizeof(blist));

		strcpy(blist.realname, target);
		blist.read     = 0xffffffff;
		blist.write    = 0xffffffff;
		blist.basenote = 0xffffffff;
	}
	strcpy(old_dosname, blist.dosname);

	for (;;) {
		dispmenu(&blist);

		print("変更する項目番号 : ");
		if (gets(buf, 2) == 0) {
			if (cquery("終了しますか", Q_YES, uid) == Q_YES)
				break;
		}

		num = buf[0] - '0';
		if (num >= 1 && num <= ELEMS(functab))
			functab[num - 1](&blist, head);
	}
	if (cquery("書き込みますか", Q_NONE, uid) == Q_NO)
		puts("-- 破棄しました --");
	else {
		lockbbs(uid, head);
		if (search_board(&btmp, target, head->us.p.config) != 0) {
			blist.last_update = btmp.last_update;
			blist.msgcount = btmp.msgcount;
		}

		if (newboard) {
			if (make_bdir(blist.dosname, head->us.p.config) != 0) {
				puts("** ディレクトリ作成に失敗しました **");
				goto error;
			}
		} else {
			if (rename_bdir(old_dosname,blist.dosname,head->us.p.config) != 0){
				puts("** ディレクトリのリネームに失敗しました **");
				goto error;
			}
		}
		if (save_boardlist(&blist, target, head->us.p.config) == 0)
			puts("** 書き込みに失敗しました **");
		else {
			for (i = 1; i <= head->us.p.maxusers; i++) {
				if (strcmp(head[i].us.u.blist.realname, target) == 0)
					memcpy(&head[i].us.u.blist, &blist, sizeof(blist));
			}
			puts("-- 保存しました --");
		}
	error:
		unlockbbs(uid, head);
	}
}

void dispmenu(boardlist far *blist)
{
	putchar('\n');

	print("    last update : ");
		disptime(blist->last_update);
	printf("    messages    : %ld\n", blist->msgcount);

	printf("[1] boardname   : %s\n", FAR(blist->realname));
	printf("[2] description : %s\n", FAR(blist->desc));
	printf("[3] dosname     : %s\n", FAR(blist->dosname));

	print("[4] read        : ");
		dispflag(blist->read);
	print("[5] write       : ");
		dispflag(blist->write);
	print("[6] basenote    : ");
		dispflag(blist->basenote);

	putchar('\n');
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

void edit_boardname(boardlist far *blist, uid_struct far *head)
{
	char buf[BOARDNAME_MAX + 1];

	printf("boardname : ");
	if (gets(buf, sizeof(buf)) != 0)
		strcpy(blist->realname, buf);
}

void edit_boardtitle(boardlist far *blist, uid_struct far *head)
{
	char buf[BOARDTITLE_MAX + 1];

	printf("description : ");
	gets(buf, sizeof(buf));
	strcpy(blist->desc, buf);
}

void edit_dosname(boardlist far *blist, uid_struct far *head)
{
	char buf[9];

	printf("dosname : ");
	if (gets(buf, sizeof(buf)) != 0) {
		if (check_dosname(buf, head->us.p.config) == 0)
			strcpy(blist->dosname, buf);
		else
			puts("** 同名のディレクトリを既に使用中です **");
	}
}

void edit_read(boardlist far *blist, uid_struct far *head)
{
	char argbuf[28];

	sprintf(argbuf, "%04X%04X 進入許可フラグ設定",
		FP_SEG(&blist->read), FP_OFF(&blist->read));

	spawn(P_WAIT, "flagedit.dlm", argbuf);
}

void edit_write(boardlist far *blist, uid_struct far *head)
{
	char argbuf[32];

	sprintf(argbuf, "%04X%04X 書き込み許可フラグ設定",
		FP_SEG(&blist->write), FP_OFF(&blist->write));

	spawn(P_WAIT, "flagedit.dlm", argbuf);
}

void edit_basenote(boardlist far *blist, uid_struct far *head)
{
	char argbuf[40];

	sprintf(argbuf, "%04X%04X ベースノート作成許可フラグ設定",
		FP_SEG(&blist->basenote), FP_OFF(&blist->basenote));

	spawn(P_WAIT, "flagedit.dlm", argbuf);
}

int make_bdir(char far *dosname, config_t far *conf)
{
	char buf[128];

	sprintf(buf, "%s\\%s", FAR(conf->dir.bbsdir), dosname);
	return mkdir(buf);
}

int check_dosname(char far *dosname, config_t far *conf)
{
	int handle;
	boardlist blist;

	handle = dopen(conf->dir.bbsdir, "blist.dat", FA_READ);
	if (handle == 0)
		return 0;
	while(read(&blist, sizeof(blist), handle)) {
		if (strcmp(blist.dosname, dosname) == 0)
			return 1;
	}
	close(handle);
	return 0;
}

int rename_bdir(char far *old, char far *new, config_t far *conf)
{
	char buf1[128], buf2[128];

	if (strcmp(old, new) == 0)
		return 0;

	sprintf(buf1, "%s\\%s", FAR(conf->dir.bbsdir), old);
	sprintf(buf2, "%s\\%s", FAR(conf->dir.bbsdir), new);

	return rename(buf1, buf2);
}

