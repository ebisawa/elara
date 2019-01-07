/*
 * proc.h - ÉvÉçÉZÉXä«óù
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(PROC_H)
#define PROC_H

#include "xstruc.h"

extern unsigned npid;
extern proc_list *CurProc;

int create_proc(char *module, char *argv, int pid);
int delete_proc(unsigned pid, int pwakeup);
void start_proc(void);
void stop_proc(void);
void change_proc(void);
int set_priority(unsigned pid, unsigned prio);
unsigned get_priority(unsigned pid);
void set_status(unsigned pid, unsigned newstat);
void wait_event(int kind, int pram);
void raise_event(int kind, int param);
void clean_deleted_proc(void);
void set_uid(unsigned pid, void *uid);
unsigned get_firstpid(void);
unsigned get_nextpid(unsigned pid);
unsigned get_ppid(unsigned pid);
char *get_name(unsigned pid);
void *get_uid(unsigned pid);
unsigned get_stat(unsigned pid);
char *get_device(unsigned pid);
unsigned get_wait1(unsigned pid);
unsigned get_wait2(unsigned pid);
void *get_stack(unsigned pid);
long get_memuse(unsigned pid);
char *get_argv(unsigned pid);
int get_stdio(unsigned pid, char *in, char *out);
int set_stdio(unsigned pid, char *in, char *out);
int get_pflag(unsigned pid);
int set_pflag(unsigned pid, unsigned flag);
int sleep_proc(unsigned pid);
int wakeup_proc(unsigned pid);
int send_message(unsigned pid, long command, char *buf);
int receive_message(unsigned *pid, long *command, char **buf);
int check_message(void);

#endif	/* PROC_H */

