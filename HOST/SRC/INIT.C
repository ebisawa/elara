
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef struct {
	char *command;
	void (*func)(char far *);
} etable_struct;

void x_device(char far *s);
void x_exec(char far *s);
void x_nowait(char far *s);
void x_stdio(char far *s);

etable_struct etable[] = {
	{ "device", x_device },		/* デバドラ組み込み */
	{ "exec",   x_exec   },		/* モジュール実行   */
	{ "nowait", x_nowait },		/* モジュール実行(バックグラウンド) */
	{ "stdio",  x_stdio  },		/* 標準入出力の設定 */
};

void main(void)
{
	int i, handle;
	char buf[1024], far *p;

	if ((handle = open("startup", FA_READ)) == 0) {
		disp("init: ファイル [startup] がオープンできません\r\n");
		exit();
	}
	disp("Starting Elara ");
	spawn(P_WAIT, "version.dlm", 0);
	disp(". please wait...\r\n\n");

	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);
		if (buf[0] == '#' || (p = strspl(buf, "\t ")) == 0)
			continue;
		for (i = 0; i < ELEMS(etable); i++) {
			if (strcmp(etable[i].command, buf) == 0)
				etable[i].func(p);
		}
	}
	close(handle);
}

void x_device(char far *s)
{
	char far *p;

	if ((p = strchr(s, ' ')) != 0) {
		while (*p == ' ')
			*p++ = 0;
	}
	if (adddrv(s, p) != 0) {
		disp("init: ");
		disp(s);
		disp(" の組込みに失敗しました\r\n");
	}
}

void x_exec(char far *s)
{
	char far *p;

	if ((p = strchr(s, ' ')) != 0) {
		while (*p == ' ')
			*p++ = 0;
	}
	if (spawn(P_WAIT, s, p) != 0) {
		disp("init: ");
		disp(s);
		disp(" の起動に失敗しました\r\n");
	}
}

void x_nowait(char far *s)
{
	char far *p;

	if ((p = strchr(s, ' ')) != 0) {
		while (*p == ' ')
			*p++ = 0;
	}
	if (spawn(P_NOWAIT, s, p) != 0) {
		disp("init: ");
		disp(s);
		disp(" の起動に失敗しました\r\n");
	}
}

void x_stdio(char far *s)
{
	setstdio(getpid(), s, s);
}

