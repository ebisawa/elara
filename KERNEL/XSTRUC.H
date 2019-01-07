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
	unsigned r_sp;			/* <!> �ړ�, �ύX���Ȃ�����     */
	unsigned r_ss;			/* <!> �ړ�, �ύX���Ȃ�����     */
	unsigned stack;			/* �X�^�b�N�̃Z�O�����g�A�h���X */
	unsigned mid;			/* ���W���[��ID                 */
	unsigned pid;			/* �v���Z�XID                   */
	unsigned ppid;			/* �e�v���Z�XID                 */
	unsigned children;		/* �q�v���Z�X��                 */
	unsigned status;		/* �X�e�[�^�X                   */
	unsigned flags;			/* �t���O                       */
	long memuse;			/* �������g�p��                 */
	char *argv;				/* �I�v�V����������             */
	char *stack_p;			/* �X�^�b�N                     */
	wait_struc wait;		/* �҂��Ă���C�x���g           */
	msg_queue msgq;			/* ���b�Z�[�W�L���[             */

	void *uid;				/* ���[�U�[ID                   */
	unsigned priority;		/* �v���Z�X�D��x               */
	device_list *d_stdin;	/* �W�����̓f�o�C�X             */
	device_list *d_stdout;	/* �W���o�̓f�o�C�X             */

	proc_list *next_a;
	proc_list *next_s;
};

#endif	/* XSTRUC_H */
