/*
 * defs.h - ê›íËíl
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(DEFS_H)
#define DEFS_H

#define PARA(size)		((size + 15) / 16)
#define ELEMS(array)	(sizeof(array) / sizeof(array[0]))

#define K_SUCCESS		0
#define K_GENERR		200
#define K_NOINIT		201
#define K_FSERR			202
#define K_PANIC			255

#define PSTATUS_MAX		4
#define S_RUN			0
#define S_WAIT			1
#define S_SLEEP			2
#define S_DELETED		3

#define W_USER			1
#define W_CHILD			2
#define W_MEMORY		3
#define W_FILE			4

#define SYSCALL_INTNO	0x80

#define P_WAIT			0
#define P_NOWAIT		1

#define P_DONTKILL		0x0001

#define PRIO_MAX		65535
#define PRIO_MIN		1

#define FA_READ			0x0000
#define FA_WRITE		0x0001
#define FA_RW			0x0002
#define FA_APPEND		0x0004
#define FA_AUTODEL		0x8000

#define FS_SET			0x0000
#define FS_CUR			0x0001
#define FS_END			0x0002

#define E_SUCCESS		0
#define E_NOMDL			1
#define E_MANYPROC		2
#define E_PARAMERR		3
#define E_NOMEM			4
#define E_NODRV			5
#define E_USING			6
#define E_NODEV			7
#define E_DRVERR		8
#define E_NOPROC		9
#define E_NOMSG			10

typedef struct time_struct {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
	int week;
} time_struct;

#endif	/* DEFS_H */

