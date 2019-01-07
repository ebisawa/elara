
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

int Running, PID, Prio;
uid_struct far *Head_u;

void already_running(void);
int isconnect(char far *device);
int check_dsr(char far *device);
void pw_dsr(uid_struct far *uid, int ch);
void pw_dcd(uid_struct far *uid, int ch);
void init_uid_struct(char far *argv);
int kill_user(uid_struct far *uid);
void initialize(uid_struct far *uid, int ch);
void setchstat(uid_struct far *uid, int stat);
void pushprio(int newprio);
void popprio(void);

void main(void)
{
	int i;
	char far *argv = getargv();

	if (Running > 0)
		already_running();
	if (argv == 0)
		return;

	Running++;
	PID = getpid();
	init_uid_struct(argv);
	setuid(PID, Head_u);
	setstdio(PID, 0, 0);
	setprio(PID, DEF_PRIO_PWATCH);
	raise(W_CHILD, PID);
	while (Head_u->us.p.init_done == 0)
		relcpu();			/* 環境設定が終了するまで待つ */
	kill_lock(1);

	for (;;) {
		for (i = 1; i <= Head_u->us.p.maxusers; i++) {
			pw_dsr(&Head_u[i], i);
			pw_dcd(&Head_u[i], i);
		}
		if (Head_u->us.p.aprio != 0)
			setprio(PID, Head_u->us.p.aprio / (Head_u->us.p.maxusers * 2));
	}
}

void already_running(void)
{
	exit();
}

int isconnect(char far *device)
{
	unsigned stat;

	if (ioctl(device, IOCTL_GETLINESTAT, &stat) != 0)
		return 0;

	return stat & LSTAT_DCD;
}

int check_dsr(char far *device)
{
	unsigned stat;

	if (ioctl(device, IOCTL_GETLINESTAT, &stat) != 0)
		return 0;

	return stat & LSTAT_DSR;
}

void pw_dsr(uid_struct far *uid, int ch)
{
	if (check_dsr(uid->us.u.device_name) != 0) {
		if (uid->chstat == CS_OFF) {
			pushprio(1);
			initialize(uid, ch);		/* 電源投入時初期化 */
			popprio();
		}
	} else {
		if (uid->chstat != CS_OFF && uid->chstat != CS_SHUTDOWN) {
			pushprio(1);
			kill_user(uid);
			setchstat(uid, CS_OFF);
			popprio();
		}
	}
}

void pw_dcd(uid_struct far *uid, int ch)
{
	if (!isconnect(uid->us.u.device_name)) {
		switch (uid->chstat) {
		case CS_LOGIN:
		case CS_USING:
			pushprio(1);
			kill_user(uid);
			initialize(uid, ch);
			popprio();
		}
	} else {
		if (uid->chstat == CS_WAIT) {
			pushprio(1);
			setstdio(PID, uid->us.u.device_name, uid->us.u.device_name);
			/*
			 * ここで setuid() しちゃうと、pwatch の uid != Head_u の時、
			 * getuid_n("pwatch") されるかもしれないのでやらないこと
			 */
			setchstat(uid, CS_LOGIN);
			if (spawn(P_NOWAIT, "login.dlm", 0) != 0) {
				puts("## login が起動できません ##");
				disconnect_device(uid->us.u.device_name);
			}
			setstdio(PID, 0, 0);
			popprio();
		}
	}
}

void init_uid_struct(char far *argv)
{
	int i, devices = nelems(argv);
	char far *p;

	if ((Head_u = xmalloc(sizeof(uid_struct) * (devices + 1))) == 0) {
		puts("## チャンネル管理用メモリ領域が確保できません ##");
		return;
	}
	memset(Head_u, 0, sizeof(uid_struct) * (devices + 1));
	Head_u->us.p.maxusers = devices;

	p = argv;
	for (i = 1; i <= devices; i++) {
		strcpy(Head_u[i].us.u.device_name, p);
		p = nextelem(p);
	}
}

int n_devices(char far *argv)
{
	int count = 0;

	while (*argv != 0) {
		while (isspace(*argv))
			*argv++ = 0;
		if (!isspace(*argv) && *argv != 0) {
			count++;
			while (!isspace(*argv) && *argv != 0)
				argv++;
		}
	}
	return count;
}

int kill_user(uid_struct far *uid)
{
	int count = 0, dr;
	unsigned pid, npid, mypid = getpid();
	char in[9], out[9];

again:
	pid = firstpid(); npid = nextpid(pid);
	while (pid != 0) {
		if (getstdio(pid, in, out) == E_NOPROC)
			goto again; /* チェックしようとしたプロセスがチェック前に死亡 */
		if (pid != mypid) {
			/* login は uid をセットしないで起動される */
			if (getuid(pid) == uid || strcmp(in, uid->us.u.device_name) == 0) {
				if (kill(pid) != 0)
					goto again;		/* 念のためやり直し */

				count++;
			}
		}
		pid = npid;
		npid = nextpid(pid);
	}
	/*
	 * ユーザーデータの保存はここで行う。
	 * 各プロセスは回線切断後、pwatch が切断を検出するまで待っていること。
	 */
	if (uid->chstat == CS_LOGIN) {			/* login中に強制切断 */
		if (uid->us.u.logoff == 0) {
			save_log_nouser(uid, Head_u,uid->us.u.connect_time, time(),
														DR_CARRIER_DOWN);
		}
	} else if (uid->chstat == CS_USING) {
		uid->us.u.udat.last_login = uid->us.u.login_time;
		uid->us.u.udat.last_logout = time();

		if (checkopt("chkdup", uid->us.u.udat.usrflag, Head_u) != 0) {
			save_usrdat(&uid->us.u.udat,
				uid->us.u.udat.id, Head_u->us.p.config);
		}
		if (uid->us.u.logoff == 0)
			uid->us.u.logoff = DR_CARRIER_DOWN;

		save_log_off(uid,Head_u,uid->us.u.udat.last_logout, uid->us.u.logoff);
	}
	memset(&uid->us.u.udat, 0, sizeof(uid->us.u.udat));
	memset(&uid->us.u.dh,   0, sizeof(uid->us.u.dh));
	uid->us.u.logoff = 0;

	return count;
}

void initialize(uid_struct far *uid, int ch)
{
	/*
	 * sendinit が起動できない時の対策のため CS_WAIT 状態にしておく
	 * ２重起動防止にも使われる
	 */
	setchstat(uid, CS_WAIT);
	spawn(P_NOWAIT, "sendinit.dlm", uid->us.u.device_name);
}

void setchstat(uid_struct far *uid, int stat)
{
	uid->chstat = stat;
	Head_u->us.p.monitor_redraw = 1;;
}

void pushprio(int newprio)
{
	Prio = getprio(PID);
	setprio(PID, newprio);
}

void popprio(void)
{
	setprio(PID, Prio);
}

