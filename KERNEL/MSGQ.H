/*
 * message.h - プロセス間通信
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(MESSAGE_H)
#define MESSAGE_H

typedef struct msgq_struct msgq_struct;

struct msgq_struct {
	unsigned from_pid;
	long command;
	void far *buffer;

	msgq_struct *prev;
	msgq_struct *next;
};

#endif	/* MESSAGE_H */

