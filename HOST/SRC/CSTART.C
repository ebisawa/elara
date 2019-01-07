
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	unsigned pid;
	uid_struct far *head;

	if ((head = getuid_n("pwatch")) == 0) {
		disp("## pwatch が存在しません ##\r\n");
		return;
	}
	if (head->us.p.config == 0) {
		disp("## 設定ファイルが読み込まれていません ##\r\n");
		return;
	}
	if (head->us.p.flagdat == 0) {
		disp("## フラグデータが読み込まれていません ##\r\n");
		return;
	}
	if (head->us.p.command == 0) {
		disp("## コマンドテーブルが読み込まれていません ##\r\n");
		return;
	}
	if (head->us.p.option == 0) {
		disp("## オプションテーブルが読み込まれていません ##\r\n");
		return;
	}
	if (getpid_n("aprio") == 0)
		return;
	if ((pid = getpid_n("tlimit")) != 0)
		setprio(pid, head->us.p.config->prio_tlimit);
	if ((pid = getpid_n("pwatch")) != 0)
		setprio(pid, head->us.p.config->prio_pwatch);
	if ((head->us.p.chatroom = xmalloc(
			sizeof(chatroom_t) * head->us.p.config->chatrooms)) == 0) {
		disp("## チャットルーム管理テーブルが確保できません ##\r\n");
		return;
	}

	head->us.p.start_time = time();
	head->us.p.monitor_redraw = 1;
	head->us.p.init_done = 1;
}

