
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef struct {
	unsigned ch;
	unsigned tmpno;
	unsigned long start;		/*  0 = topline */
	unsigned long end;			/* -1 = bottomline */
} range_t;

typedef struct {
	int cmdcode;
	int (*func)(int, char far *argv, range_t far *, uid_struct far *, uid_struct far *);
} cmdtab_t;

void editor(int ch, char far *argv, uid_struct far *head, uid_struct far *uid);
int open_tmp(int tmpno, int ch, int del, unsigned mode, uid_struct far *head);
char far *skip_space(char far *s);
int set_range(range_t far *range, char far *buf);
int input_range(range_t far *range);
int inputline(int handle, int startline, uid_struct far *uid);

int command_q(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_n(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_w(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_u(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_a(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_p(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_l(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_r(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_d(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_i(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int command_c(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);
int disp_help(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head);

cmdtab_t cmdtable[] = {
	{ '?', disp_help },
	{ 'a', command_a },
	{ 'c', command_c },
	{ 'd', command_d },
	{ 'i', command_i },
	{ 'l', command_l },
	{ 'n', command_n },
	{ 'p', command_p },
	{ 'r', command_r },
	{ 'u', command_u },
	{ 'q', command_q },
	{ 'w', command_w },
	{ 'y', command_w },
};

void main(void)
{
	int ch;
	char far *argv;
	uid_struct far *head, far *uid;

	if ((argv = getargv()) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;
	if ((head = getuid_n("pwatch")) == 0)
		return;

	ch = getchnum(uid->us.u.device_name, head);
	editor(ch, argv, head, uid);
}

void editor(int ch, char far *argv, uid_struct far *head, uid_struct far *uid)
{
	int i, handle, dst;
	char far *p, buf[64], filename[128];
	range_t range;

	if ((handle = open_tmp(0, ch, 1, FA_RW, head)) == 0)
		return;
	if ((dst = open(argv, FA_READ)) != 0) {
		filecopy_h(dst, handle);
		close(dst);
	}
	range.ch = ch;
	range.tmpno = 0;
	goto label0;

error:
	puts("type ? for help");

label0:
	for (;;) {
		if (uid->us.u.udat.term.esc & ESC_COLOR)
			print(CYAN);
		printf("[%c]EDITOR>", (uid->us.u.udat.autosign) ? '*' : ' ');
		if (uid->us.u.udat.term.esc & ESC_COLOR)
			print(WHITE);

		if (gets(buf, sizeof(buf)) != 0)
			break;
	}
	p = skip_space(buf);
	range.start = 0;
	range.end = 0;
	if (set_range(&range, &p[1]) != 0)
		goto error;
	for (i = 0; i < ELEMS(cmdtable); i++) {
		if (cmdtable[i].cmdcode == p[0]) {
			handle = cmdtable[i].func(handle, argv, &range, uid, head);
			goto label0;
		}
	}
	goto error;
}

int open_tmp(int tmpno, int ch, int del, unsigned mode, uid_struct far *head)
{
	char filename[128];
	config_t far *conf = head->us.p.config;

	sprintf(filename, "%s\\edtmp%d.%03d", conf->dir.tmpdir, tmpno, ch);
	if (del)
		remove(filename);
	return open(filename, mode);
}

char far *skip_space(char far *s)
{
	while (isspace(*s))
		s++;

	return s;
}

int set_range(range_t far *range, char far *buf)
{
	int i;
	unsigned long ltmp = 0;

	if (buf[0] == 0)
		return 0;
	for (i = 0; buf[i] != ',' && buf[i] != 0; i++) {
		if (isdigit(buf[i])) {
			ltmp = lmul(ltmp, 10);
			ltmp += buf[i] - '0';
		} else if (isspace(buf[i]))
			continue;
		else
			return 1;
	}
	range->start = ltmp;

	if (buf[i] == ',') {
		i++;
		for (ltmp = 0; buf[i] != 0; i++) {
			if (isdigit(buf[i])) {
				ltmp = lmul(ltmp, 10);
				ltmp += buf[i] - '0';
			} else if (isspace(buf[i]))
				continue;
			else
				return 1;
		}
	}
	range->end = (ltmp == 0) ? ~0 : ltmp;
	return 0;
}

int input_range(range_t far *range)
{
	int len;
	char buf[20];

	print("範囲 : ");
	if ((len = gets(buf, sizeof(buf))) == 0)
		return 0;

	set_range(range, buf);
	return len;
}


int command_q(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	if (cquery("編集結果を破棄しますか", Q_NONE, uid) == Q_YES) {
		close(handle);
		puts("-- 編集中断 --");
		exit();
	}
	return handle;
}

int command_n(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	close(handle);
	puts("-- 編集中断 --");
	exit();
}

int command_w(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	int dst;

	if ((dst = open(argv, FA_WRITE)) == 0) {
		puts("** 書き込みエラー **");
		exit();
	}
	filecopy_h(handle, dst);
	close(handle);
	close(dst);
	exit();
}

int command_u(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	int len;
	char buf[128];

	seek(handle, 0L, FS_END);
	cputs("-- 行頭に[.]で終了します --", CYAN, uid);
	for (;;) {
		len = gets(buf, sizeof(buf));
		if (buf[0] == '.' && buf[1] == 0)
			return handle;

		write(buf, len, handle);
		write("\r\n", 2, handle);
	}
}

int command_a(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	char buf[128];
	unsigned long count = 0;

	seek(handle, 0L, FS_SET);
	while (fgets(buf, sizeof(buf), handle))
		count++;

	return inputline(handle, count + 1, uid);
}

int inputline(int handle, int startline, uid_struct far *uid)
{
	int len, pcw;
	char buf[128];
	unsigned long lineno = startline;

	cputs("-- 行頭に[.]で終了します --", CYAN, uid);
	for (;;) {
		pcw = printf("%lu: ", lineno);
		len = gets3(buf, sizeof(buf), 0, pcw);
		if (buf[0] == '.' && buf[1] == 0)
			return handle;

		write(buf, len, handle);
		write("\r\n", 2, handle);
		lineno++;
	}
	return handle;
}

int command_p(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	char buf[128];
	long linecount;

	if (range->start == 0 && range->end == 0)
		input_range(range);
	seek(handle, 0L, FS_SET);
	for (linecount = 1; linecount <= range->end; linecount++) {
		if (fgets(buf, sizeof(buf), handle) == 0)
			break;
		if (linecount >= range->start)
			print(buf);
	}
	return handle;
}

int command_l(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	char buf[128];
	long linecount;

	if (range->start == 0 && range->end == 0)
		input_range(range);
	seek(handle, 0L, FS_SET);
	for (linecount = 1; linecount <= range->end; linecount++) {
		if (fgets(buf, sizeof(buf), handle) == 0)
			break;
		if (linecount >= range->start)
			printf("%ld: %s", linecount, FAR(buf));
	}
	return handle;
}

int command_d(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	int len, newhandle;
	char buf[128];
	unsigned long count = 1;

	if (range->start == 0 && range->end == 0)
		input_range(range);
	if (cquery("削除しますか", Q_NONE, uid) == Q_NO) {
		return handle;
	}
	if ((newhandle = open_tmp(range->tmpno^1, range->ch, 1, FA_RW, head)) == 0)
		return handle;

	range->tmpno ^= 1;
	seek(handle, 0L, FS_SET);
	while ((len = fgets(buf, sizeof(buf), handle)) != 0) {
		if (count < range->start || count > range->end)
			write(buf, len, newhandle);

		count++;
	}
	close(handle);
	return newhandle;
}

int command_r(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	return handle;
}

int command_i(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	int len, pcw, tmphandle, newhandle;
	char buf[128];
	unsigned long count;

	if (range->start == 0 && range->end == 0) {
		print("何行目の前に挿入しますか ? : ");
		gets(buf, 11);
		range->start = atol(buf);
	}
	if ((tmphandle = open_tmp(9, range->ch, 1, FA_RW | FA_AUTODEL, head)) == 0)
		return handle;

	count = 1;
	if ((newhandle = open_tmp(range->tmpno^1, range->ch,1,FA_RW,head)) == 0) {
		close(tmphandle);
		return handle;
	}
	range->tmpno ^= 1;
	seek(handle, 0L, FS_SET);
	while (count < range->start) {
		if ((len = fgets(buf, sizeof(buf), handle)) == 0)
			break;

		write(buf, len, newhandle);
		count++;
	}
	inputline(tmphandle, count, uid);
	seek(tmphandle, 0L, FS_SET);

	while ((len = fgets(buf, sizeof(buf), tmphandle)) != 0)
		write(buf, len, newhandle);
	while ((len = fgets(buf, sizeof(buf), handle)) != 0)
		write(buf, len, newhandle);

	close(tmphandle);
	close(handle);
	return newhandle;
}

int command_c(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	int nh;

	if ((nh = open_tmp(range->tmpno ^ 1, range->ch, 0, FA_READ, head)) == 0)
		return handle;

	close(nh);
	if ((nh = open_tmp(range->tmpno ^ 1, range->ch, 0, FA_RW, head)) == 0)
		return handle;

	close(handle);
	range->tmpno ^= 1;
	return nh;
}

int disp_help(int handle, char far *argv, range_t far *range, uid_struct far *uid, uid_struct far *head)
{
	typetext(head->us.p.config->dir.helpdir, "editor.hlp");
	return handle;
}

