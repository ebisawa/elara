
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
		disp("## pwatch �����݂��܂��� ##\r\n");
		return;
	}
	if (head->us.p.config == 0) {
		disp("## �ݒ�t�@�C�����ǂݍ��܂�Ă��܂��� ##\r\n");
		return;
	}
	if (head->us.p.flagdat == 0) {
		disp("## �t���O�f�[�^���ǂݍ��܂�Ă��܂��� ##\r\n");
		return;
	}
	if (head->us.p.command == 0) {
		disp("## �R�}���h�e�[�u�����ǂݍ��܂�Ă��܂��� ##\r\n");
		return;
	}
	if (head->us.p.option == 0) {
		disp("## �I�v�V�����e�[�u�����ǂݍ��܂�Ă��܂��� ##\r\n");
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
		disp("## �`���b�g���[���Ǘ��e�[�u�����m�ۂł��܂��� ##\r\n");
		return;
	}

	head->us.p.start_time = time();
	head->us.p.monitor_redraw = 1;
	head->us.p.init_done = 1;
}
