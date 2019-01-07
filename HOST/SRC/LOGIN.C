
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

int check_password(char far *password, userdat far *u, uid_struct far *h);
uid_struct far *search_uidp(uid_struct far *head);
void check_pwerr(uid_struct far *uid);
void check_duplicate(uid_struct far *uid, uid_struct far *head);
void do_disconnect(uid_struct far *head);
void inc_access_count(uid_struct far *uid, uid_struct far *head);
unsigned get_tlimit(uid_struct far *uid, uid_struct far *head);

void main(void)
{
	int i, nouser;
	char id[ID_MAX + 1], password[PASSWORD_MAX + 1];
	long connect_time;
	uid_struct far *head, far *uid, far *p;
	userdat u;

	connect_time = time();
	if ((head = getuid(getpid())) == 0)
		return;
	if ((p = search_uidp(head)) == 0)
		return;

	p->us.u.connect_time = connect_time;
	typetext(head->us.p.config->dir.msgdir, "connect.msg");
	uid = search_uidp(head);

	for (i = 0; i < 4; i++) {
		nouser = 0;
		print("ID : ");
		gets2(id, sizeof(id), 0);

		if (search_usrdat(&u, id, head->us.p.config) == 0)
			nouser = 1;
		if (nouser == 1 || checkopt("chkpw", u.usrflag, head) == 1) {
			print("Password : ");
			gets2(password, sizeof(password), '?');

			if (nouser || check_password(password, &u, head) == 0)
				continue;
		}
		if (uid != 0) {
			memcpy(&uid->us.u.udat, &u, sizeof(userdat));

			/* パスワード一致 */
			check_duplicate(uid, head);
			inc_access_count(uid, head);

			check_pwerr(uid);
			setuid(getpid(), uid);

			uid->chstat = CS_USING;
			head->us.p.monitor_redraw = 1;

			uid->us.u.login_time = time();
			uid->us.u.tlimit = get_tlimit(uid, head);

			uid->us.u.sequencer = uid->us.u.udat.last_msgread;
			uid->us.u.open_flag = 0;
			uid->us.u.telegram_received = 0;
			uid->us.u.chat = 0;
			exec(uid->us.u.udat.shell, 0);		/* 正常なら戻ってこない */
		}
		/* アクセスカウント増えちゃうけど仕様だ */

		puts("\n## システムエラー ##");
		save_log_off(uid, head, time(), DR_SYSTEM_ERROR);
		p->us.u.logoff = DR_SYSTEM_ERROR;
		do_disconnect(head);
	}
	save_log_nouser(uid, head, connect_time, time(), DR_CANNOT_LOGIN);
	p->us.u.logoff = DR_CANNOT_LOGIN;	/* CarrierDown との区別に利用 */
	do_disconnect(head);
}

int check_password(char far *password, userdat far *u, uid_struct far *h)
{
	/*
	 * ID, Password が一致するまで uid->us.u.udat はセットしないこと
	 */
	if (strcmp(u->password, password) != 0) {
		u->pwerr++;
		save_usrdat(u, u->id, h->us.p.config);
		return 0;
	}
	return 1;
}

uid_struct far *search_uidp(uid_struct far *head)
{
	char in[9], out[9];

	getstdio(getpid(), in, out);
	return getuid_d(in, head);
}

void check_pwerr(uid_struct far *uid)
{
	if (uid->us.u.udat.pwerr != 0) {
		if (uid->us.u.udat.term.esc & ESC_COLOR)
			print(RED);

		printf("\nPassword Error : %u\n", uid->us.u.udat.pwerr);

		if (uid->us.u.udat.term.esc & ESC_COLOR)
			print(WHITE);
		uid->us.u.udat.pwerr = 0;
	}
}

void check_duplicate(uid_struct far *uid, uid_struct far *head)
{
	int i;
	accesslog last;

	if (checkopt("chkdup", uid->us.u.udat.usrflag, head) == 0)
		return;
	for (i = 1; i <= head->us.p.maxusers; i++) {
		if (stricmp(head[i].us.u.udat.id, uid->us.u.udat.id) == 0) {
			if (&head[i] != uid) {
				puts("\n## 使用中ID ##");

				last_access(&last, head->us.p.config);
				uid->us.u.total_access = last.count;

				/* duplicate の場合、カウントは増やさない */
				save_log_preset(uid, head, uid->us.u.total_access);
				save_log_off(uid, head, time(), DR_DUPLICATE);
				uid->us.u.logoff = DR_DUPLICATE;	/* CD との区別に利用 */
				do_disconnect(head);
			}
		}
	}
}

void do_disconnect(uid_struct far *head)
{
	disconnect();
	waitsec(1, head);

	for (;;)
		relcpu();
}

void inc_access_count(uid_struct far *uid, uid_struct far *head)
{
	accesslog last;

	last_access(&last, head->us.p.config);
	uid->us.u.total_access = last.count + 1;
	uid->us.u.udat.access++;
	save_log_preset(uid, head, uid->us.u.total_access);
}

unsigned get_tlimit(uid_struct far *uid, uid_struct far *head)
{
	int i;
	unsigned tlimit = 0;
	unsigned long fmask = 0x80000000;

	for (i = 0; i < 32; i++, fmask >>= 1) {
		if (uid->us.u.udat.usrflag & fmask) {
			if (head->us.p.flagdat[i].tlimit == 65535)
				return 65535;

			tlimit += head->us.p.flagdat[i].tlimit;
		}
	}
	return tlimit;
}

