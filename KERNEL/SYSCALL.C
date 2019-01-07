/*
 * syscall.c - システムコール
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dos.h>
#include <dir.h>
#include "kernel.h"
#include "proc.h"
#include "device.h"
#include "filesys.h"
#include "memory.h"

#define ENTRY_MAX	(sizeof(sys_call_tbl) / sizeof(sys_call_tbl[0]))

typedef struct regs_map {
	unsigned r_ax;
	unsigned r_bx;
	unsigned r_cx;
	unsigned r_dx;
	unsigned r_es;
	unsigned r_ds;
	unsigned r_si;
	unsigned r_di;
	unsigned r_bp;
	unsigned r_ip;
	unsigned r_cs;
	unsigned flags;
} regs_map;

typedef void (*sysfunc_t)(regs_map *);

regs_map far *Regs;

static void (interrupt far *old_vect)(void);
static char dlmext[] = ".dlm";
static char ddmext[] = ".ddm";

static void sys_relcpu(regs_map *regs);
static void sys_exec(regs_map *regs);
static void sys_spawn(regs_map *regs);
static void sys_getpid(regs_map *regs);
static void sys_getppid(regs_map *regs);
static void sys_getname(regs_map *regs);
static void sys_getstat(regs_map *regs);
static void sys_getprio(regs_map *regs);
static void sys_setprio(regs_map *regs);
static void sys_getwait1(regs_map *regs);
static void sys_getwait2(regs_map *regs);
static void sys_getstack(regs_map *regs);
static void sys_getmuse(regs_map *regs);
static void sys_getargv(regs_map *regs);
static void sys_kill(regs_map *regs);
static void sys_exit(regs_map *regs);
static void sys_disp(regs_map *regs);
static void sys_shutdown(regs_map *regs);
static void sys_adddrv(regs_map *regs);
static void sys_deldrv(regs_map *regs);
static void sys_adddev(regs_map *regs);
static void sys_deldev(regs_map *regs);
static void sys_getuid(regs_map *regs);
static void sys_setuid(regs_map *regs);
static void sys_vsprintf(regs_map *regs);
static void sys_system(regs_map *regs);
static void sys_firstpid(regs_map *regs);
static void sys_nextpid(regs_map *regs);
static void sys_sleep(regs_map *regs);
static void sys_wakeup(regs_map *regs);
static void sys_wait(regs_map *regs);
static void sys_raise(regs_map *regs);
static void sys_malloc(regs_map *regs);
static void sys_free(regs_map *regs);
static void sys_xmalloc(regs_map *regs);
static void sys_xfree(regs_map *regs);
static void sys_open(regs_map *regs);
static void sys_close(regs_map *regs);
static void sys_read(regs_map *regs);
static void sys_write(regs_map *regs);
static void sys_seek(regs_map *regs);
static void sys_ioctl(regs_map *regs);
static void sys_getstdio(regs_map *regs);
static void sys_setstdio(regs_map *regs);
static void sys_mkdir(regs_map *regs);
static void sys_rmdir(regs_map *regs);
static void sys_time(regs_map *regs);
static void sys_localtime(regs_map *regs);
static void sys_ctime(regs_map *regs);
static void sys_getpflag(regs_map *regs);
static void sys_setpflag(regs_map *regs);
static void sys_fileno(regs_map *regs);
static void sys_sendmsg(regs_map *regs);
static void sys_recvmsg(regs_map *regs);
static void sys_checkmsg(regs_map *regs);
static void sys_remove(regs_map *regs);
static void sys_close2(regs_map *regs);
static void sys_procsw(regs_map *regs);
static void sys_rename(regs_map *regs);
static void sys_iolimit(regs_map *regs);
static void sys_mktime(regs_map *regs);

static sysfunc_t sys_call_tbl[] = {
	sys_relcpu,			/*  0 */
	sys_exec,
	sys_spawn,
	sys_getpid,
	sys_getppid,
	sys_getname,		/* 5 */
	sys_getstat,
	sys_getprio,
	sys_setprio,
	sys_getwait1,
	sys_getwait2,		/* 10 */
	sys_getstack,
	sys_getmuse,
	sys_getargv,
	sys_kill,
	sys_exit,			/* 15 */
	sys_disp,
	sys_shutdown,
	sys_adddrv,
	sys_deldrv,
	sys_adddev,			/* 20 */
	sys_deldev,
	sys_getuid,
	sys_setuid,
	sys_vsprintf,
	sys_system,			/* 25 */
	sys_firstpid,
	sys_nextpid,
	sys_sleep,
	sys_wakeup,
	sys_wait,			/* 30 */
	sys_raise,
	sys_malloc,
	sys_free,
	sys_xmalloc,
	sys_xfree,			/* 35 */
	sys_open,
	sys_close,
	sys_read,
	sys_write,
	sys_seek,			/* 40 */
	sys_ioctl,
	sys_getstdio,
	sys_setstdio,
	sys_mkdir,
	sys_rmdir,			/* 45 */
	sys_time,
	sys_localtime,
	sys_ctime,
	sys_getpflag,
	sys_setpflag,		/* 50 */
	sys_fileno,
	sys_sendmsg,
	sys_recvmsg,
	sys_checkmsg,
	sys_remove,			/* 55 */
	sys_close2,
	sys_procsw,
	sys_rename,
	sys_iolimit,
	sys_mktime,			/* 60 */
};

int ProcSwitch = 1;
int In_Kernel;
int LastSysCall;
int LastSysCall2;

/* -------------------------------------------------------------------------
 * システムコール受付準備
 * ---------------------------------------------------------------------- */
void init_syscall(void)
{
	old_vect = _dos_getvect(SYSCALL_INTNO);
	_dos_setvect(SYSCALL_INTNO, sys_entry);
}

/* -------------------------------------------------------------------------
 * システムコール受付終了
 * ---------------------------------------------------------------------- */
void term_syscall(void)
{
	_dos_setvect(SYSCALL_INTNO, old_vect);
}

/* -------------------------------------------------------------------------
 * システムコールエントリ
 * -------------------------------------------------------------------------
 * <!> 再突入可能
 * ---------------------------------------------------------------------- */
void far *sys_call(void)
{
	regs_map far *regs;
	unsigned x_ss, x_sp;

	regs = Regs;
	In_Kernel++;

	if (regs->r_ax >= ENTRY_MAX)
		panic("不正なシステムコール(ax = %04Xh)", regs->r_ax);

	if (In_Kernel > 1) {
		x_ss = FP_SEG(regs);
		x_sp = FP_OFF(regs);
	} else {
		CurProc->r_ss = FP_SEG(regs);
		CurProc->r_sp = FP_OFF(regs);
	}
	LastSysCall = regs->r_ax;
	LastSysCall2 = 0;
	sys_call_tbl[regs->r_ax](regs);					/* システムコール処理 */

	clean_deleted_proc();

	if (In_Kernel <= 1 && ProcSwitch > 0)
		change_proc();								/* タスク切り替え */

	In_Kernel--;
	if (In_Kernel == 0)
		return MK_FP(CurProc->r_ss, CurProc->r_sp);		/* スタック切り替え */
	else
		return MK_FP(x_ss, x_sp);
}

void xreturn(void)
{
	unsigned o_ss, o_sp;
	unsigned *s_ss, *s_sp;

	if (In_Kernel > 1)
		panic("カーネル再入中に処理中断しようとしました");

	o_ss = CurProc->r_ss;
	o_sp = CurProc->r_sp;
	s_ss = &CurProc->r_ss;
	s_sp = &CurProc->r_sp;
	change_proc();
	In_Kernel--;
	xreturn_sub(CurProc->r_ss, CurProc->r_sp, s_ss, s_sp);

	CurProc->r_ss = o_ss;
	CurProc->r_sp = o_sp;
	In_Kernel++;
}


/* -------------------------------------------------------------------------
 * relcpu - プロセス切り替え
 * -------------------------------------------------------------------------
 * In:  AX    - 0
 * ---------------------------------------------------------------------- */
static void sys_relcpu()
{
	/* nop */
}

/* -------------------------------------------------------------------------
 * exec - モジュール実行(現在実行中のモジュールは停止)
 * -------------------------------------------------------------------------
 * In:  AX    - 1
 *      DS:BX - モジュール名(ASCIZ)
 *      ES:CX - オプション文字列(ASCIZ)
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_exec(regs_map *regs)
{
	int error;

	if ((error = exec_module_dlm(
		MK_FP(regs->r_ds, regs->r_bx), MK_FP(regs->r_es, regs->r_cx))) != 0)
		regs->r_ax = error;		/* 成功してたらこの時点でスタックはない */
}

/* -------------------------------------------------------------------------
 * spawn - モジュール実行(新たにプロセス作成)
 * -------------------------------------------------------------------------
 * In:  AX    - 2
 *      BX    - モード (P_WAIT, P_NOWAIT)
 *      DS:CX - モジュール名(ASCIZ)
 *      ES:DX - オプション文字列(ASCIZ)
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_spawn(regs_map *regs)
{
	regs->r_ax = spawn_module_dlm(
		regs->r_bx,
		MK_FP(regs->r_ds, regs->r_cx),
		MK_FP(regs->r_es, regs->r_dx));
}

/* -------------------------------------------------------------------------
 * getpid - pid の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 3
 *
 * Out: AX    - pid
 * ---------------------------------------------------------------------- */
static void sys_getpid(regs_map *regs)
{
	regs->r_ax = CurProc->pid;
}

/* -------------------------------------------------------------------------
 * getppid - 親プロセスの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 4
 *      BX    - pid
 *
 * Out: AX    - 0 = エラー, else = pid
 * ---------------------------------------------------------------------- */
static void sys_getppid(regs_map *regs)
{
	regs->r_ax = get_ppid(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * getname - モジュール名の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 5
 *      BX    - pid
 *
 * Out: DX:AX - 0 = エラー, else = モジュール名
 * ---------------------------------------------------------------------- */
static void sys_getname(regs_map *regs)
{
	char far *p = get_name(regs->r_bx);

	regs->r_dx = FP_SEG(p);
	regs->r_ax = FP_OFF(p);
}

/* -------------------------------------------------------------------------
 * getstat - プロセス状態の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 6
 *      BX    - pid
 *
 * Out: AX - 0 = エラー, else = stat
 * ---------------------------------------------------------------------- */
static void sys_getstat(regs_map *regs)
{
	regs->r_ax = get_stat(regs->r_bx);
}


/* -------------------------------------------------------------------------
 * getprio - プロセス優先度の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 7
 *      BX    - pid
 *
 * Out: AX    - priority
 * ---------------------------------------------------------------------- */
static void sys_getprio(regs_map *regs)
{
	regs->r_ax = get_priority(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * setprio - プロセス優先度の設定
 * -------------------------------------------------------------------------
 * In:  AX    - 8
 *      BX    - pid
 *      CX    - priority
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_setprio(regs_map *regs)
{
	regs->r_ax = set_priority(regs->r_bx, regs->r_cx);
}

/* -------------------------------------------------------------------------
 * getwait1 - 待っているイベントの取得(1)
 * -------------------------------------------------------------------------
 * In:  AX    - 9
 *      BX    - pid
 *
 * Out: AX    - 0 = エラー, else = wait1
 * ---------------------------------------------------------------------- */
static void sys_getwait1(regs_map *regs)
{
	regs->r_ax = get_wait1(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * getwait2 - 待っているイベントの取得(2)
 * -------------------------------------------------------------------------
 * In:  AX    - 10
 *      BX    - pid
 *
 * Out: AX    - 0 = エラー, else = wait2
 * ---------------------------------------------------------------------- */
static void sys_getwait2(regs_map *regs)
{
	regs->r_ax = get_wait2(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * getstack - スタックポインタの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 11
 *      BX    - pid
 *
 * Out: DX:AX - 0 = エラー, else = stack
 * ---------------------------------------------------------------------- */
static void sys_getstack(regs_map *regs)
{
	void far *stack = get_stack(regs->r_bx);

	regs->r_dx = FP_SEG(stack);
	regs->r_ax = FP_OFF(stack);
}

/* -------------------------------------------------------------------------
 * getmuse - 使用しているメモリ量の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 12
 *      BX    - pid
 *
 * Out: DX:AX - 0 = エラー, else = memuse
 * ---------------------------------------------------------------------- */
static void sys_getmuse(regs_map *regs)
{
	long memuse = get_memuse(regs->r_bx);

	regs->r_dx = (unsigned)(memuse >> 16);
	regs->r_ax = (unsigned)memuse;
}

/* -------------------------------------------------------------------------
 * getargv - オプション文字列の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 13
 *
 * Out: DX:AX - 0 = エラー, else = argv
 * ---------------------------------------------------------------------- */
static void sys_getargv(regs_map *regs)
{
	regs->r_dx = FP_SEG(CurProc->argv);
	regs->r_ax = FP_OFF(CurProc->argv);
}


/* -------------------------------------------------------------------------
 * kill - 指定されたプロセスを削除する
 * -------------------------------------------------------------------------
 * In:  AX    - 14
 *      BX    - pid
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_kill(regs_map *regs)
{
	regs->r_ax = delete_proc(regs->r_bx, 1);
}

/* -------------------------------------------------------------------------
 * exit - 現在実行中のプロセスを削除する
 * -------------------------------------------------------------------------
 * In:  AX    - 15
 * ---------------------------------------------------------------------- */
static void sys_exit()
{
	delete_proc(CurProc->pid, 1);
}

/* -------------------------------------------------------------------------
 * disp - 文字列表示
 * -------------------------------------------------------------------------
 * In:  AX    - 16
 *      ES:BX - 文字列(ASCIZ)
 * ---------------------------------------------------------------------- */
static void sys_disp(regs_map *regs)
{
	disp_string(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * shutdown - カーネル停止
 * -------------------------------------------------------------------------
 * In:  AX    - 17
 *      BX    - リターンコード
 * ---------------------------------------------------------------------- */
static void sys_shutdown(regs_map *regs)
{
	KernelReturnCode = regs->r_bx;
	stop_proc();
}

/* -------------------------------------------------------------------------
 * adddrv - デバイスドライバの組み込み
 * -------------------------------------------------------------------------
 * In:  AX    - 18
 *      DS:BX - ドライバ名(ASCIZ)
 *      ES:CX - オプション文字列(ASCIZ)
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_adddrv(regs_map *regs)
{
	regs->r_ax = install_driver(
		MK_FP(regs->r_ds, regs->r_bx),
		MK_FP(regs->r_es, regs->r_cx));
}

/* -------------------------------------------------------------------------
 * deldrv - デバイスドライバの解放
 * -------------------------------------------------------------------------
 * In:  AX    - 19
 *      ES:BX - ドライバ名(ASCIZ)
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_deldrv(regs_map *regs)
{
	regs->r_ax = release_driver(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * adddev - デバイスの登録
 * -------------------------------------------------------------------------
 * In:  AX    - 20
 *      DS:BX - ドライバ名(ASCIZ)
 *      ES:CX - デバイス名(ASCIZ)
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_adddev(regs_map *regs)
{
	regs->r_ax = add_device(
		MK_FP(regs->r_ds, regs->r_bx),
		MK_FP(regs->r_es, regs->r_cx));
}

/* -------------------------------------------------------------------------
 * deldev - デバイスの削除
 * -------------------------------------------------------------------------
 * In:  AX    - 21
 *      ES:BX - ドライバ名(ASCIZ)
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_deldev(regs_map *regs)
{
	regs->r_ax = delete_device(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * getuid - ユーザーID の取得
 * -------------------------------------------------------------------------
 * In:  AX    - 22
 *      BX    - pid
 *
 * Out: DX:AX - 0 = エラー, else = uid
 * ---------------------------------------------------------------------- */
static void sys_getuid(regs_map *regs)
{
	void *uid;

	uid = get_uid(regs->r_bx);
	regs->r_dx = FP_SEG(uid);
	regs->r_ax = FP_OFF(uid);
}

/* -------------------------------------------------------------------------
 * setuid - ユーザーIDのセット
 * -------------------------------------------------------------------------
 * In:  AX    - 23
 *      BX    - pid
 *      ES:CX - uid
 * ---------------------------------------------------------------------- */
static void sys_setuid(regs_map *regs)
{
	set_uid(regs->r_bx, MK_FP(regs->r_es, regs->r_cx));
}

/* -------------------------------------------------------------------------
 * vsprintf
 * -------------------------------------------------------------------------
 * In:  AX    - 24
 *      DS:BX - buf
 *      ES:CX - format
 *      SI:DX - va_list
 *
 * Out: AX    - vsprintfの戻り値
 * ---------------------------------------------------------------------- */
static void sys_vsprintf(regs_map *regs)
{
	regs->r_ax = vsprintf(
		MK_FP(regs->r_ds, regs->r_bx),
		MK_FP(regs->r_es, regs->r_cx),
		MK_FP(regs->r_si, regs->r_dx));
}

/* -------------------------------------------------------------------------
 * system - MS-DOSコマンドの実行
 * -------------------------------------------------------------------------
 * In:  AX    - 25
 *      ES:BX - 文字列
 * ---------------------------------------------------------------------- */
static void sys_system(regs_map *regs)
{
	system(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * firstpid - 最初のプロセスの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 26
 *
 * Out: AX    - pid
 * ---------------------------------------------------------------------- */
static void sys_firstpid(regs_map *regs)
{
	regs->r_ax = get_firstpid();
}

/* -------------------------------------------------------------------------
 * nextpid - 次のプロセスの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 27
 *      BX    - pid
 *
 * Out: AX    - 0 = エラー, else = pid
 * ---------------------------------------------------------------------- */
static void sys_nextpid(regs_map *regs)
{
	regs->r_ax = get_nextpid(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * sleep - プロセスをsleep状態にする
 * -------------------------------------------------------------------------
 * In:  AX    - 28
 *      BX    - pid
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_sleep(regs_map *regs)
{
	regs->r_ax = sleep_proc(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * wakeup - プロセスをsleep状態から復帰する
 * -------------------------------------------------------------------------
 * In:  AX    - 29
 *      BX    - pid
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_wakeup(regs_map *regs)
{
	regs->r_ax = wakeup_proc(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * wait - イベントの発生を待つ
 * -------------------------------------------------------------------------
 * In:  AX    - 30
 *      BX    - event
 *      CX    - param
 * ---------------------------------------------------------------------- */
static void sys_wait(regs_map *regs)
{
	wait_event(regs->r_bx, regs->r_cx);
}

/* -------------------------------------------------------------------------
 * raise - イベントを発生させる
 * -------------------------------------------------------------------------
 * In:  AX    - 31
 *      BX    - event
 *      CX    - param
 * ---------------------------------------------------------------------- */
static void sys_raise(regs_map *regs)
{
	raise_event(regs->r_bx, regs->r_cx);
}

/* -------------------------------------------------------------------------
 * malloc - メモリ確保
 * -------------------------------------------------------------------------
 * In:  AX    - 32
 *      BX    - サイズ
 *
 * Out: DX:AX - 0 = 失敗, else = 確保したメモリ
 * ---------------------------------------------------------------------- */
static void sys_malloc(regs_map *regs)
{
	regs->r_dx = umem_alloc(regs->r_bx);
	regs->r_ax = 0;
}

/* -------------------------------------------------------------------------
 * free - メモリ解放
 * -------------------------------------------------------------------------
 * In:  AX    - 33
 *      ES:BX - アドレス
 * ---------------------------------------------------------------------- */
static void sys_free(regs_map *regs)
{
	umem_free(regs->r_es, CurProc);
}

/* -------------------------------------------------------------------------
 * xmalloc - メモリ確保(自動解放なし)
 * -------------------------------------------------------------------------
 * In:  AX    - 34
 *      BX    - サイズ
 *
 * Out: DX:AX - 0 = 失敗, else = 確保したメモリ
 * ---------------------------------------------------------------------- */
static void sys_xmalloc(regs_map *regs)
{
	regs->r_dx = umem_alloc_x(regs->r_bx);
	regs->r_ax = 0;
}

/* -------------------------------------------------------------------------
 * xfree - メモリ解放(自動解放なし)
 * -------------------------------------------------------------------------
 * In:  AX    - 35
 *      ES:BX - アドレス
 * ---------------------------------------------------------------------- */
static void sys_xfree(regs_map *regs)
{
	umem_free_x(regs->r_es);
}

/* -------------------------------------------------------------------------
 * open - ファイルオープン
 * -------------------------------------------------------------------------
 * In:  AX    - 36
 *      ES:BX - ファイル名
 *      CX    - モード
 *
 * Out: AX    - 0 = 失敗, else = ハンドル
 * ---------------------------------------------------------------------- */
static void sys_open(regs_map *regs)
{
	regs->r_ax = open_file(MK_FP(regs->r_es, regs->r_bx), regs->r_cx);
}

/* -------------------------------------------------------------------------
 * close - ファイルクローズ
 * -------------------------------------------------------------------------
 * In:  AX    - 37
 *      BX    - ハンドル
 * ---------------------------------------------------------------------- */
static void sys_close(regs_map *regs)
{
	close_file(regs->r_bx, 0);
}

/* -------------------------------------------------------------------------
 * read - ファイル/デバイスリード
 * -------------------------------------------------------------------------
 * In:  AX    - 38
 *      ES:BX - バッファ
 *      CX    - サイズ
 *      DX    - ハンドル (0 = stdin)
 *
 * Out: AX    - 0 = 失敗, else = 読み込んだサイズ
 * ---------------------------------------------------------------------- */
static void sys_read(regs_map *regs)
{
	char far *p;

	if (regs->r_cx == 0 || (p = MK_FP(regs->r_es, regs->r_bx)) == 0) {
		regs->r_ax = 0;
		return;
	}
	if (regs->r_dx == 0) {
		if (CurProc->d_stdin == 0)
			regs->r_ax = 0;
		else {
			if (CurProc->d_stdin->iolimit != 0
					&& CurProc->d_stdin->iolimit != CurProc->pid) {
				regs->r_ax = 0;
				return;
			}
			if (CurProc->d_stdin->driver == 0)
				panic("I/O ERROR(stdin/driver)");
			if (CurProc->d_stdin->device_p == 0)
				panic("I/O ERROR(stdin/device)");
			regs->r_ax = CurProc->d_stdin->driver->func.read(
							p, regs->r_cx, CurProc->d_stdin->device_p);
			LastSysCall2 = 1;
		}
	} else {
		regs->r_ax = read_file(p, regs->r_cx, regs->r_dx);
		LastSysCall2 = 2;
	}
}

/* -------------------------------------------------------------------------
 * write - ファイル/デバイスライト
 * -------------------------------------------------------------------------
 * In:  AX    - 39
 *      ES:BX - バッファ
 *      CX    - サイズ
 *      DX    - ハンドル (0 = stdout)
 *
 * Out: AX    - 0 = 失敗, else = 書き込んだサイズ
 * ---------------------------------------------------------------------- */
static void sys_write(regs_map *regs)
{
	char far *p;

	if (regs->r_cx == 0 || (p = MK_FP(regs->r_es, regs->r_bx)) == 0) {
		regs->r_ax = 0;
		return;
	}
	if (regs->r_dx == 0) {
		if (CurProc->d_stdout == 0)
			regs->r_ax = regs->r_cx;
		else {
			if (CurProc->d_stdout->iolimit != 0
					 && CurProc->d_stdout->iolimit != CurProc->pid) {
				regs->r_ax = 0;
				return;
			}
			if (CurProc->d_stdout->driver == 0)
				panic("I/O ERROR(stdout/driver)");
			if (CurProc->d_stdout->device_p == 0)
				panic("I/O ERROR(stdout/device)");
			regs->r_ax = CurProc->d_stdout->driver->func.write(
							p, regs->r_cx, CurProc->d_stdout->device_p);
			LastSysCall2 = 1;
		}
	} else {
		regs->r_ax = write_file(p, regs->r_cx, regs->r_dx);
		LastSysCall2 = 2;
	}
}

/* -------------------------------------------------------------------------
 * seek - ファイルシーク
 * -------------------------------------------------------------------------
 * In:  AX    - 40
 *      BX    - ハンドル
 *      CX:DX - 移動量
 *      SI    - モード
 *
 * Out: DX:AX - 0 = 失敗, else = 移動後の位置
 * ---------------------------------------------------------------------- */
static void sys_seek(regs_map *regs)
{
	long pos;

	pos = (long)regs->r_cx << 16 | regs->r_dx;
	pos = seek_file(regs->r_bx, pos, regs->r_si);

	regs->r_dx = (unsigned)(pos >> 16);
	regs->r_ax = (unsigned)pos;
}

/* -------------------------------------------------------------------------
 * ioctl - I/O CONTROL
 * -------------------------------------------------------------------------
 * In:  AX    - 41
 *      DS:BX - デバイス名
 *      CX    - command
 *      ES:DX - param
 *
 * Out: AX    - 返却値
 * ---------------------------------------------------------------------- */
static void sys_ioctl(regs_map *regs)
{
	regs->r_ax = ioctl_device(
		MK_FP(regs->r_ds, regs->r_bx),
		regs->r_cx,
		MK_FP(regs->r_es, regs->r_dx));
}

/* -------------------------------------------------------------------------
 * getstdio - 標準入出力デバイスの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 42
 *      BX    - pid
 *      DS:CX - stdin
 *      ES:DX - stdut
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_getstdio(regs_map *regs)
{
	regs->r_ax = get_stdio(
		regs->r_bx,
		MK_FP(regs->r_ds, regs->r_cx),
		MK_FP(regs->r_es, regs->r_dx));
}

/* -------------------------------------------------------------------------
 * setstdio - 標準入出力デバイスの設定
 * -------------------------------------------------------------------------
 * In:  AX    - 43
 *      BX    - pid
 *      DS:CX - stdin
 *      ES:DX - stdut
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_setstdio(regs_map *regs)
{
	regs->r_ax = set_stdio(
		regs->r_bx,
		MK_FP(regs->r_ds, regs->r_cx),
		MK_FP(regs->r_es, regs->r_dx));
}

/* -------------------------------------------------------------------------
 * mkdir - ディレクトリの作成
 * -------------------------------------------------------------------------
 * In:  AX    - 44
 *      ES:BX - path
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_mkdir(regs_map *regs)
{
	regs->r_ax = mkdir(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * rmdir - ディレクトリの削除
 * -------------------------------------------------------------------------
 * In:  AX    - 45
 *      ES:BX - path
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_rmdir(regs_map *regs)
{
	regs->r_ax = rmdir(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * time - 現在時刻を得る
 * -------------------------------------------------------------------------
 * In:  AX    - 46
 *
 * Out: DX:AX - 0 = 成功, else = 現在時刻
 * ---------------------------------------------------------------------- */
static void sys_time(regs_map *regs)
{
	long time_val = time(0);

	regs->r_dx = (unsigned)(time_val >> 16);
	regs->r_ax = (unsigned)time_val;
}

/* -------------------------------------------------------------------------
 * localtime - 現在時刻を得る
 * -------------------------------------------------------------------------
 * In:  AX    - 47
 *      ES:BX - time_struct far *
 *      CX:DX - time_t
 * ---------------------------------------------------------------------- */
static void sys_localtime(regs_map *regs)
{
	long t;
	struct tm *tm_p;
	time_struct *ts_p;

	t = (long)regs->r_cx << 16 | regs->r_dx;
	ts_p = MK_FP(regs->r_es, regs->r_bx);

	tm_p = localtime(&t);
	ts_p->year  = tm_p->tm_year;
	ts_p->month = tm_p->tm_mon + 1;
	ts_p->day   = tm_p->tm_mday;
	ts_p->hour  = tm_p->tm_hour;
	ts_p->min   = tm_p->tm_min;
	ts_p->sec   = tm_p->tm_sec;
	ts_p->week  = tm_p->tm_wday;
}

/* -------------------------------------------------------------------------
 * ctime - 現在時刻を得る
 * -------------------------------------------------------------------------
 * In:  AX    - 48
 *      BX:CX - time_t
 *
 * Out: DX:AX - 0 = 失敗, else = バッファへのポインタ
 * ---------------------------------------------------------------------- */
static void sys_ctime(regs_map *regs)
{
	long t;
	char *buf;

	t = (long)regs->r_bx << 16 | regs->r_cx;

	buf = ctime(&t);
	regs->r_dx = FP_SEG(buf);
	regs->r_ax = FP_OFF(buf);
}

/* -------------------------------------------------------------------------
 * getpflag - プロセスフラグの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 49
 *      BX    - pid
 *
 * Out: AX    - 0 = 失敗, else = flag
 * ---------------------------------------------------------------------- */
static void sys_getpflag(regs_map *regs)
{
	regs->r_ax = get_pflag(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * setpflag - プロセスフラグの設定
 * -------------------------------------------------------------------------
 * In:  AX    - 50
 *      BX    - pid
 *      CX    - flag
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_setpflag(regs_map *regs)
{
	regs->r_ax = set_pflag(regs->r_bx, regs->r_cx);
}

/* -------------------------------------------------------------------------
 * fileno - DOS ファイルハンドルの取得
 * -------------------------------------------------------------------------
 * In:  AX    - 51
 *      BX    - handle
 *
 * Out: AX    - 0 = 失敗, else = doshandle
 * ---------------------------------------------------------------------- */
static void sys_fileno(regs_map *regs)
{
	regs->r_ax = get_doshandle(regs->r_bx);
}

/* -------------------------------------------------------------------------
 * sendmsg - メッセージ送信
 * -------------------------------------------------------------------------
 * In:  AX    - 52
 *      BX    - 送り先PID
 *      CX:DX - command
 *      ES:SI - buffer
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_sendmsg(regs_map *regs)
{
	long cmd = (long)regs->r_cx << 16 | regs->r_dx;

	regs->r_ax = send_message(regs->r_bx, cmd, MK_FP(regs->r_es, regs->r_si));
}

/* -------------------------------------------------------------------------
 * recvmsg - メッセージ受信
 * -------------------------------------------------------------------------
 * In:  AX    - 53
 *      DS:BX - 送り元PID
 *      ES:CX - command
 *      SI:DX - buffer
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_recvmsg(regs_map *regs)
{
	regs->r_ax = receive_message(
		MK_FP(regs->r_ds, regs->r_bx),
		MK_FP(regs->r_es, regs->r_cx),
		MK_FP(regs->r_si, regs->r_dx));
}

/* -------------------------------------------------------------------------
 * checkmsg - メッセージチェック
 * -------------------------------------------------------------------------
 * In:  AX    - 54
 *
 * Out: AX    - 受信しているメッセージの数
 * ---------------------------------------------------------------------- */
static void sys_checkmsg(regs_map *regs)
{
	regs->r_ax = check_message();
}

/* -------------------------------------------------------------------------
 * remove - ファイル削除
 * -------------------------------------------------------------------------
 * In:  AX    - 55
 *      ES:BX - path
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_remove(regs_map *regs)
{
	regs->r_ax = remove(MK_FP(regs->r_es, regs->r_bx));
}

/* -------------------------------------------------------------------------
 * close2 - ファイルクローズ
 * -------------------------------------------------------------------------
 * In:  AX    - 56
 *      BX    - ハンドル
 *      CX    - フラグマスク
 * ---------------------------------------------------------------------- */
static void sys_close2(regs_map *regs)
{
	close_file(regs->r_bx, regs->r_cx);
}

/* -------------------------------------------------------------------------
 * procsw - プロセス切り替えの許可/禁止
 * -------------------------------------------------------------------------
 * In:  AX    - 57
 *      BX    - 0 = 禁止, else = 許可
 * ---------------------------------------------------------------------- */
static void sys_procsw(regs_map *regs)
{
	regs->r_ax = ProcSwitch;
	ProcSwitch = regs->r_bx;
}

/* -------------------------------------------------------------------------
 * rename - ファイル名変更
 * -------------------------------------------------------------------------
 * In:  AX    - 58
 *      DS:BX - oldname
 *      ES:CX - newname
 *
 * Out: AX    - 0 = 成功, else = 失敗
 * ---------------------------------------------------------------------- */
static void sys_rename(regs_map *regs)
{
	regs->r_ax = rename(
		MK_FP(regs->r_ds, regs->r_bx),
		MK_FP(regs->r_es, regs->r_cx));
}

/* -------------------------------------------------------------------------
 * iolimit - デバイス使用可能プロセス制限
 * -------------------------------------------------------------------------
 * In:  AX    - 59
 *      ES:BX - device name
 *      CX    - pid
 * ---------------------------------------------------------------------- */
static void sys_iolimit(regs_map *regs)
{
	regs->r_ax = iolimit_device(MK_FP(regs->r_es, regs->r_bx), regs->r_cx);
}

/* -------------------------------------------------------------------------
 * mktime
 * -------------------------------------------------------------------------
 * In:  AX    - 60
 *      ES:BX - time_struct far *
 * Out: DX:AX - time_t
 * ---------------------------------------------------------------------- */
static void sys_mktime(regs_map *regs)
{
	long l;
	struct tm t;
	time_struct *ts_p;

	ts_p = MK_FP(regs->r_es, regs->r_bx);
	memset(&t, 0, sizeof(t));

	t.tm_year = ts_p->year;
	t.tm_mon  = ts_p->month - 1;
	t.tm_mday = ts_p->day;
	t.tm_hour = ts_p->hour;
	t.tm_min  = ts_p->min;
	t.tm_sec  = ts_p->sec;

	l = mktime(&t);

	regs->r_dx = (unsigned)(l >> 16);
	regs->r_ax = (unsigned)l;
}

