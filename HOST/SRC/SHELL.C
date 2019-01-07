
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "telegram.h"

#define MENULEVEL_MAX	8
#define MENUPROMPT_MAX	32
#define BUFLEN			64
#define BUFLEN			128
#define MAIN			"MAIN"

int menuproc(char far *menuname, char far *menuparam, int level,
	char far *vbuf, uid_struct far *uid, uid_struct far *head, unsigned ch);
void disp_menu(char far *menuname, config_t far *conf);
void make_filename(char far *buf, char far *filename, char far *extension);
void disp_prompt(char far *menuname, char far *menuparam, uid_struct far *uid);
void disp_menuname(char far *menuname, char far *menuparam);
void read_menuprompt(char far *buf, char far *menuname, config_t far *conf);
int multi_statement(char far *buf, char far *menuname, char far *param,
	int level, int far *menu_disp, int nocheck, unsigned ch,
	uid_struct far *uid, uid_struct far *head);
int command(char far *cmd, char far *option,
	char far *menuname, char far *param,
	int level, int far *menu_disp, int nocheck, unsigned ch,
	uid_struct far *uid, uid_struct far *head);
int isnumstr(char far *str);
int menunum(char far *buf, char far *menuname, int num, config_t far *conf);
void exoption(char far *buf, char far *option, char far *param);
void command_help(char far *cmd, config_t far *conf);
int search_command(char far *cmd, int nocheck,
	uid_struct far *uid, uid_struct far *head);
void error(char far *cmd);
int exec_command(char far *cmd, char far *option);
int menu_exist(char far *menuname, config_t far *conf);
void command_list(char far *cmd, uid_struct far *uid, uid_struct far *head);
void disp_telegram(uid_struct far *head, unsigned ch);
void remove_telegram(uid_struct far *head, unsigned ch);

void main(void)
{
	unsigned ch;
	uid_struct far *head, far *uid;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	ch = getchnum(uid->us.u.device_name, head);
	remove_telegram(head, ch);
	typetext(head->us.p.config->dir.msgdir, "login.msg");
	menuproc(MAIN, 0, 1, 0, uid, head, ch);
}

int menuproc(char far *menuname, char far *menuparam, int level,
	char far *vbuf, uid_struct far *uid, uid_struct far *head, unsigned ch)
{
	int c, trflag = 0, menu_disp = (level == 1) ? 0 : 1;
	char menuprompt[MENUPROMPT_MAX], buf[BUFLEN];

	if (level > MENULEVEL_MAX) {
		puts("** メニュー階層が深すぎます **");
		return 0;
	}
	read_menuprompt(menuprompt, menuname, head->us.p.config);

	for (;;) {
		strcpy(uid->us.u.ucmd, menuname);
		strupr(uid->us.u.ucmd);

		if (uid->us.u.telegram_received) {
			puts("-- 電報が届きました --");
			disp_telegram(head, ch);
			puts("-- 以上です --\n");
		}
		if ((uid->us.u.udat.expert == 1 || uid->us.u.udat.expert == 2)
			 													&& menu_disp)
			disp_menu(menuname, head->us.p.config);

		menu_disp = 0;
		if (vbuf == 0) {
			disp_prompt(menuprompt, menuparam, uid);
			while ((c = getchar()) == -1) {
				if (uid->us.u.telegram_received) {
					if (trflag == 0)
						puts("\n-- 電報が届きました --");
					disp_telegram(head, ch);
					trflag = 1;
				}
			}
			if (trflag) {
				puts("-- 以上です --\n");
				disp_prompt(menuprompt, menuparam, uid);
				trflag = 0;
			}
			ungetch(c);
			if (gets(buf, sizeof(buf)) == 0) {
				menu_disp++;
				continue;
			}
		} else {
			strcpy(buf, vbuf);
			vbuf = 0;
		}
		if (buf[0] == '.' || buf[0] == '/') {
			if (level == 1)
				puts("-- トップメニューです --");
			else
				return buf[0];
		} else {
			if (multi_statement(buf, menuname, menuparam, level,
									&menu_disp, 0, ch, uid, head) == '/') {
				if (level != 1)
					return '/';
			}
		}
	}
}

void disp_menu(char far *menuname, config_t far *conf)
{
	char filename[13];

	make_filename(filename, menuname, "txt");
	typetext(conf->dir.menudir, filename);
}

void make_filename(char far *buf, char far *filename, char far *extension)
{
	strcpy(buf, filename);
	strcat(buf, ".");
	strcat(buf, extension);
}

void disp_prompt(char far *menuname, char far *menuparam, uid_struct far *uid)
{
	char far *prompt = uid->us.u.udat.term.prompt;

	while (*prompt != 0) {
		switch (*prompt) {
		case '$':
			disp_menuname(menuname, menuparam);
			break;
		default:
			putchar(*prompt);
			break;
		}
		prompt++;
	}

}

void disp_menuname(char far *menuname, char far *menuparam)
{
	while (*menuname != 0) {
		switch (*menuname) {
		case '%':
			if (menuparam != 0)
				print(menuparam);

			break;
		default:
			putchar(*menuname);
			break;
		}
		menuname++;
	}
}

void read_menuprompt(char far *buf, char far *menuname, config_t far *conf)
{
	int handle;
	char filename[13];

	*buf = 0;
	make_filename(filename, menuname, "mnu");

	if ((handle = dopen(conf->dir.menudir, filename, FA_READ)) == 0)
		return;

	fgets(buf, MENUPROMPT_MAX, handle);
	chop(buf);
	close(handle);
}

int multi_statement(char far *buf, char far *menuname, char far *param,
	int level, int far *menu_disp, int nocheck, unsigned ch,
	uid_struct far *uid, uid_struct far *head)
{
	char far *mp1, far *mp2, far *option;
	char tmp[BUFLEN];

	mp1 = buf;
	while (mp1 != 0) {
		while (isspace(*mp1))
			mp1++;
		mp2 = strspl(mp1, ";");
		option = strspl(mp1, " ");

		if (isnumstr(mp1)) {
			if (menunum(tmp, menuname, atou(mp1), head->us.p.config)) {
				if (multi_statement(tmp, menuname, param, level,
									menu_disp, 1, ch, uid, head) == '/')
					return '/';
			}
		} else {
			if (option != 0)
				exoption(tmp, option, param);
			else
				tmp[0] = 0;

			if (menu_exist(mp1, head->us.p.config)) {
				(*menu_disp)++;
				return menuproc(mp1, tmp, level + 1, mp2, uid, head, ch);
			}
			*menu_disp += command(mp1, tmp, menuname, param,
								level, menu_disp, nocheck, ch, uid, head);
		}
		mp1 = mp2;
	}
	return 0;
}

/* メニューを表示する必要のある時は 0 以外を返す */
int command(char far *cmd, char far *option,
	char far *menuname, char far *param,
	int level, int far *menu_disp, int nocheck, unsigned ch,
	uid_struct far *uid, uid_struct far *head)
{
	int n, i, len = strlen(cmd);
	char buf[BUFLEN];

	if (cmd[len - 1] == '?') {
		cmd[len - 1] = 0;
		command_list(cmd, uid, head);

		return 0;
	}
	if (strstr(option, "-?") != 0) {
		command_help(cmd, head->us.p.config);
		return 0;
	}
	if ((n = search_command(cmd, nocheck, uid, head)) == -1) {
		error(cmd);
		return -1;
	}
	if (head->us.p.command[n].alias != 0) {
		strcpy(buf, head->us.p.command[n].alias);
		multi_statement(buf, menuname, option, level,
								menu_disp, 1, ch, uid, head);
	} else {
		strcpy(uid->us.u.ucmd, head->us.p.command[n].name);
		strupr(uid->us.u.ucmd);
		if (exec_command(head->us.p.command[n].name, option) != 0) {
			error(cmd);
			return -1;
		}
		putchar('\n');
	}
	return 0;
}

int menu_exist(char far *menuname, config_t far *conf)
{
	int handle;
	char filename[13];

	if (strlen(menuname) > 8)
		return 0;

	make_filename(filename, menuname, "mnu");
	if ((handle = dopen(conf->dir.menudir, filename, FA_READ)) == 0)
		return 0;

	close(handle);
	return 1;
}

int isnumstr(char far *str)
{
	while (*str != 0) {
		if (!isdigit(*str))
			return 0;

		str++;
	}
	return 1;
}

int menunum(char far *buf, char far *menuname, int num, config_t far *conf)
{
	int i, handle;
	char filename[13];

	*buf = 0;
	if (num <= 0)
		return 0;

	make_filename(filename, menuname, "mnu");
	if ((handle = dopen(conf->dir.menudir, filename, FA_READ)) == 0)
		return 0;

	for (i = 0; i < num + 1; i++) {
		if (fgets(buf, BUFLEN, handle) == 0) {
			i = 0;
			break;
		}
	}
	close(handle);
	chop(buf);

	return i;
}

void exoption(char far *buf, char far *option, char far *param)
{
	while (*option != 0) {
		if (*option != '%')
			*buf++ = *option;
		else {
			if (param != 0) {
				strcpy(buf, param);
				buf += strlen(param);
			}
		}
		option++;
	}
	*buf = 0;
}

void command_help(char far *cmd, config_t far *conf)
{
	char filename[13];

	make_filename(filename, cmd, "usg");
	typetext(conf->dir.helpdir, filename);
}

int search_command(char far *cmd, int nocheck,
	uid_struct far *uid, uid_struct far *head)
{
	int i, len, r = -1;

	for (i = 0; i < head->us.p.maxcmd; i++) {
		len = strlen(cmd);
		if (strncmp(head->us.p.command[i].name, cmd, len) == 0) {
			if ((head->us.p.command[i].flag & uid->us.u.udat.usrflag) == 0 && !nocheck)
				continue;
			if (len == strlen(head->us.p.command[i].name))
				return i;			/* 完全一致 */
			if (r != -1)
				return -1;			/* 2 つ以上一致してはいけない */
			else
				r = i;
		}
	}
	return r;
}

void error(char far *cmd)
{
	printf("-- コマンド [%s] は存在しません --\n", cmd);
}

int exec_command(char far *cmd, char far *option)
{
	char filename[13];

	make_filename(filename, cmd, "dlm");
	return spawn(P_WAIT, filename, (option[0] != 0) ? option : 0);
}

void command_list(char far *cmd, uid_struct far *uid, uid_struct far *head)
{
	int handle;
	char far *p, buf[BUFLEN];

	handle = dopen(head->us.p.config->dir.helpdir, "command.hlp", FA_READ);
	if (handle == 0)
		return;

	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);

		if ((p = strspl(buf, "\t ")) == 0)
			continue;
		if (cmd[0] != 0 && strncmp(buf, cmd, strlen(cmd)) != 0)
			continue;
		if (search_command(buf, 0, uid, head) == -1)
			continue;

		printf("%-8s : %s\n", FAR(buf), p);
	}
	close(handle);
	putchar('\n');
}

void disp_telegram(uid_struct far *head, unsigned ch)
{
	int handle;
	char filename[13];
	time_struct t;
	telegram_t telg;

	head[ch].us.u.telegram_received = 0;
	sprintf(filename, "telegram.%03d", ch);
	handle = dopen(head->us.p.config->dir.tmpdir, filename, FA_READ);
	if (handle == 0)
		return;

	while (read(&telg, sizeof(telegram_t), handle)) {
		localtime(&t, telg.time_stamp);
		printf("*from [%d] %s (%s)  %02d:%02d\n",
			telg.from_ch, FAR(telg.from_id), FAR(telg.from_handle),
			t.hour, t.min);
		printf("\"%s\"\n", FAR(telg.message));
	}
	close(handle);
	remove_telegram(head, ch);
}

void remove_telegram(uid_struct far *head, unsigned ch)
{
	char filename[128];

	sprintf(filename, "%s\\telegram.%03d",
		FAR(head->us.p.config->dir.tmpdir), ch);
	remove(filename);
}

