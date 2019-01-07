
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "telegram.h"

uid_struct far *Head;		/* これはどのチャンネルでも共通だから共有可 */

int send_telegram(char far *msg, unsigned to_ch, unsigned from_ch, uid_struct far *uid);
int append_telegram(telegram_t far *telg, unsigned to_ch);
int list_user(unsigned from_ch);

void main(void)
{
	unsigned from_ch, to_ch;
	char far *argv, far *msg;
	char buf[TELEMSG_MAX + 1], to_id[ID_MAX + 1];
	uid_struct far *uid;

	if ((Head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	argv = getargv();
	if (argv == 0)
		msg = "";
	else {
		msg = argv;
		while (*msg != 0) {
			if (*msg == ' ') {
				msg++;
				break;
			}
			msg++;
		}
	}
	from_ch = getchnum(uid->us.u.device_name, Head);
	to_ch = (argv != 0) ? atou(argv) : 0;
	if (to_ch == 0) {
		if (list_user(from_ch) == 0)
			return;
		print("電報の送り先は ? : ");
		if (gets(buf, 3) == 0)
			return;
		to_ch = atou(buf);
	}
	if (to_ch > Head->us.p.maxusers)
		return;
	if (Head[to_ch].chstat != CS_USING)
		return;
	if (Head[to_ch].us.u.telegram_received > 64) {
		puts("-- 宛て先の電報バッファが一杯です --");
		return;
	}
	strcpy(to_id, Head[to_ch].us.u.udat.id);
	if (*msg == 0) {
		printf("電文[>%s] : ", FAR(Head[to_ch].us.u.udat.handle));
		if (gets(buf, sizeof(buf)) == 0)
			return;
		msg = buf;
	}
	if (Head[to_ch].chstat != CS_USING
						|| stricmp(Head[to_ch].us.u.udat.id, to_id) != 0) {
		puts("-- 宛て先ユーザーがログオフしました --");
		return;
	}
	if (send_telegram(msg, to_ch, from_ch, uid) != 0)
		print("-- 電報を送りました --");
	else
		puts("** 電報送信に失敗しました **");
}

int send_telegram(char far *msg, unsigned to_ch, unsigned from_ch, uid_struct far *uid)
{
	telegram_t telg;

	memset(&telg, 0, sizeof(telg));
	telg.from_ch = from_ch;
	strcpy(telg.from_id, uid->us.u.udat.id);
	strcpy(telg.from_handle, uid->us.u.udat.handle);
	telg.time_stamp = time();
	strcpy(telg.message, msg);
	if (append_telegram(&telg, to_ch) == 0)
		return 0;

	return ++Head[to_ch].us.u.telegram_received;
}

int append_telegram(telegram_t far *telg, unsigned to_ch)
{
	int handle;
	char filename[13];

	sprintf(filename, "telegram.%03d", to_ch);
	handle = dopen(Head->us.p.config->dir.tmpdir, filename, FA_APPEND);
	if (handle == 0)
		return 0;

	write(telg, sizeof(telegram_t), handle);
	close(handle);
	return 1;
}

int list_user(unsigned from_ch)
{
	int i, u = 0;

	for (i = 1; i <= Head->us.p.maxusers; i++) {
		if (Head[i].chstat == CS_USING && i != from_ch) {
			printf("[%d]%s  ", i, Head[i].us.u.udat.id);
			u++;
		}
	}
	if (u > 0)
		putchar('\n');

	return u;
}

