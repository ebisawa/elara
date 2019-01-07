/*
 * xstruc.h
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(XSTRUC_H)
#define XSTRUC_H

#include "msgq.h"

typedef struct driver_list driver_list;
typedef struct device_list device_list;
typedef struct wait_struc wait_struc;
typedef struct msg_queue msg_queue;
typedef struct proc_list proc_list;

typedef struct iof_struct {
	int (far *init)(char far *argv);
	int (far *term)(void);
	void far *(far *open)(char far *device_name);
	void (far *close)(void far *device_p);
	unsigned (far *read)(char far *buf, unsigned size, void far *device_p);
	unsigned (far *write)(char far *buf, unsigned size, void far *device_p);
	int (far *ioctl)(char far *device_name, unsigned command, void far *param);
} iof_struct;

struct driver_list {
	char driver_name[9];
	unsigned udevs;
	unsigned segm;
	unsigned size;
	iof_struct func;
	driver_list *next;
};

struct device_list {
	char device_name[9];
	unsigned ucount;
	unsigned iolimit;
	void *device_p;
	driver_list *driver;
	device_list *next;
};

struct wait_struc {
	int kind;
	int param;
};

struct msg_queue {
	unsigned nmsgs;
	msgq_struct *head;
	msgq_struct *tail;
};

struct proc_list {
	unsigned r_sp;			/* <!> 移動, 変更しないこと     */
	unsigned r_ss;			/* <!> 移動, 変更しないこと     */
	unsigned stack;			/* スタックのセグメントアドレス */
	unsigned mid;			/* モジュールID                 */
	unsigned pid;			/* プロセスID                   */
	unsigned ppid;			/* 親プロセスID                 */
	unsigned children;		/* 子プロセス数                 */
	unsigned status;		/* ステータス                   */
	unsigned flags;			/* フラグ                       */
	long memuse;			/* メモリ使用量                 */
	char *argv;				/* オプション文字列             */
	char *stack_p;			/* スタック                     */
	wait_struc wait;		/* 待っているイベント           */
	msg_queue msgq;			/* メッセージキュー             */

	void *uid;				/* ユーザーID                   */
	unsigned priority;		/* プロセス優先度               */
	device_list *d_stdin;	/* 標準入力デバイス             */
	device_list *d_stdout;	/* 標準出力デバイス             */

	proc_list *next_a;
	proc_list *next_s;
};

#endif	/* XSTRUC_H */

