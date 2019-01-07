/*
 * proc.c - プロセス管理
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "kernel.h"
#include "xstruc.h"
#include "proc.h"
#include "device.h"
#include "filesys.h"
#include "memory.h"
#include "msgq.h"

#define STACK_SIZE		4096

unsigned npid, LoopCount;
unsigned long KernelStack;
proc_list *CurProc;

static proc_list *ProcListA;
static proc_list *ProcListS[PSTATUS_MAX];

extern void far x_start_proc(proc_list far *p);
extern void far x_stop_proc(void);
static void chain_msg(proc_list *p, msgq_struct *msg);
static void chain_a(proc_list *p);
static void unchain_a(proc_list *p);
static void chain_s(proc_list *p);
static void unchain_s(proc_list *p);
static unsigned makepid(void);
static proc_list *pid2procp(unsigned pid);
static unsigned *initstack(unsigned stack, unsigned ics, unsigned ids);
static void noparent(proc_list *p);
static void delete_body(unsigned pid);
static void message_lost(proc_list *p);

int create_proc(char *module, char *argv, int pid)
{
	int error;
	unsigned *s;
	proc_list *p;
	mcb_struct *mcb;

	if ((p = kmem_alloc(sizeof(proc_list))) == 0)
		return E_NOMEM;

	memset(p, 0, sizeof(proc_list));
	if ((p->mid = load_module_dlm(module)) == 0) {
		error = E_NOMDL;
		goto error3;
	}
	if ((p->stack = dosmem_alloc(PARA(STACK_SIZE))) == 0) {
		error = E_NOMEM;
		goto error2;
	}
	p->stack_p = MK_FP(p->stack, 0);
	*p->stack_p = 0;
	/* memset(p->stack_p, 0, PARA(STACK_SIZE) * 16); */

	if (pid != 0)
		p->pid = pid;
	else {
		if ((p->pid = makepid()) == 0) {
			error = E_MANYPROC;
			goto error1;
		}
	}
	if (CurProc == 0)
		p->priority = PRIO_MIN;
	else {
		p->uid      = CurProc->uid;
		p->ppid     = CurProc->pid;
		p->priority = CurProc->priority;
		CurProc->children++;
		if (CurProc->d_stdin != 0)
			p->d_stdin  = open_device(CurProc->d_stdin->device_name);
		if (CurProc->d_stdout != 0)
			p->d_stdout = open_device(CurProc->d_stdout->device_name);
	}
	p->status = S_RUN;

	if (argv != 0)
		p->argv = strdup(argv);

	s = initstack(p->stack, p->mid, p->mid);
	p->r_sp = FP_OFF(s);
	p->r_ss = FP_SEG(s);

	chain_a(p);
	chain_s(p);

	npid = p->pid;

	mcb = MK_FP(p->stack - 1, 0);
	mcb->d.nx.zero = 0;
	mcb->d.nx.nextsegm = 0;
	mcb->d.nx.xflag = 0;

	return 0;

error1:
	dosmem_free(p->stack);
error2:
	free_module_dlm(p->mid);
error3:
	kmem_free(p);

	return error;
}

int delete_proc(unsigned pid, int pwakeup)
{
	proc_list *p, *q;

	if ((p = pid2procp(pid)) == 0)
		return E_NOPROC;
	if (p->flags & P_DONTKILL && pid != CurProc->pid)
		return E_NOPROC;

	if (p->children != 0) {
		noparent(p);
		p->children = 0;
	}
	if (p->ppid != 0) {
		if ((q = pid2procp(p->ppid)) != 0)
			q->children--;
		else
			panic("存在するはずの親が存在しません(pid = %d, ppid = %d)", pid, p->ppid);

		p->ppid = 0;
	}
	p->priority = 0;
	if (p->argv != 0) {
		kmem_free(p->argv);
		p->argv = 0;
	}
	p->r_ss = 0;
	p->r_sp = 0;

	close_device(p->d_stdin);
	close_device(p->d_stdout);
	p->d_stdin = 0;
	p->d_stdout = 0;

	if (p->msgq.nmsgs != 0)
		message_lost(p);

	umem_free_proc(p);
	close_file_pid(pid);

	if (pwakeup == 1)
		raise_event(W_CHILD, pid);
	if (pid == CurProc->pid)
		set_status(pid, S_DELETED);
	else
		delete_body(pid);

	if (ProcListS[S_RUN] == 0)
		stop_proc();

	return 0;
}

static void delete_body(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return;
	if (p->stack != 0)
		dosmem_free(p->stack);
	if (p->mid != 0)
		free_module_dlm(p->mid);

	unchain_a(p);
	unchain_s(p);
	kmem_free(p);
}

static void message_lost(proc_list *p)
{
	proc_list *from;
	msgq_struct *msg;
	mcb_struct *mcb;
	unsigned segm;

	for (msg = p->msgq.head; msg != 0; msg = msg->next) {
		segm = FP_SEG(msg->buffer);
		mcb = MK_FP(segm - 1, 0);
		if ((mcb->id == 'M' || mcb->id == 'Z') && mcb->d.nx.zero == 0) {
			if (mcb->d.nx.xflag == 1)
				umem_free_x(segm);
			else
				umem_free(segm, p);
		}
		kmem_free(msg);			/* 本当は危険なのかも */
	}
}

void start_proc(void)
{
	if (ProcListS[S_RUN] == 0)
		panic("プロセスが１つも作成されていません");
	if ((CurProc = ProcListS[S_RUN]) == 0)
		return;

	x_start_proc(CurProc);
}

void stop_proc(void)
{
	x_stop_proc();		/* main からは呼ばないこと */
}

void change_proc(void)
{
	if (ProcListS[S_RUN] == 0)
		panic("実行可能プロセスがありません");

	for (;;) {
		if ((CurProc = CurProc->next_a) == 0) {
			CurProc = ProcListA;
			LoopCount++;
		}
		if (CurProc->status == S_RUN) {
			if (CurProc->priority == 0)
				panic("優先度が 0 で実行可能なプロセスがあります(pid = %d)", CurProc->pid);
			if (*CurProc->stack_p != 0)
				panic("スタックオーバーフロー(pid = %d)", CurProc->pid);
			if ((LoopCount % CurProc->priority) == 0)
				break;
		}
	}
}

int set_priority(unsigned pid, unsigned prio)
{
	proc_list *t;

	if ((t = pid2procp(pid)) == 0)
		return E_NOPROC;

	if (prio > PRIO_MAX)
		return E_PARAMERR;
	if (prio < PRIO_MIN)
		return E_PARAMERR;

	if (prio == 0)
		prio = 1;

	t->priority = prio;
	return 0;
}

unsigned get_priority(unsigned pid)
{
	proc_list *t;

	if ((t = pid2procp(pid)) == 0)
		return 0;

	return t->priority;
}

void set_status(unsigned pid, unsigned newstat)
{
	proc_list *t;

	if ((t = pid2procp(pid)) == 0)
		return;

	unchain_s(t);
	t->status = newstat;
	chain_s(t);
}

void wait_event(int kind, int param)
{
	CurProc->wait.kind = kind;
	CurProc->wait.param = param;
	set_status(CurProc->pid, S_WAIT);
}

void raise_event(int kind, int param)
{
	proc_list *p;

	for (p = ProcListS[S_WAIT]; p != 0; p = p->next_s) {
		if (p->wait.kind == kind && p->wait.param == param) {
			p->wait.kind = 0;
			p->wait.param = 0;
			set_status(p->pid, S_RUN);
		}
	}
}

void clean_deleted_proc(void)
{
	while (ProcListS[S_DELETED] != 0) {
		if (ProcListS[S_DELETED]->pid == CurProc->pid)
			break;

		delete_body(ProcListS[S_DELETED]->pid);
	}
}

void set_uid(unsigned pid, void *uid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) != 0)
		p->uid = uid;
}

unsigned get_firstpid(void)
{
	return ProcListA->pid;
}

unsigned get_nextpid(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) != 0 && p->next_a != 0)
		return p->next_a->pid;

	return 0;
}

unsigned get_ppid(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) != 0)
		return p->ppid;

	return 0;
}

char *get_name(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return module_name(p->mid);
}

void *get_uid(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return p->uid;
}

unsigned get_stat(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return p->status;
}

unsigned get_wait1(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return p->wait.kind;
}

unsigned get_wait2(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return p->wait.param;
}

void far *get_stack(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return MK_FP(p->r_ss, p->r_sp);
}

long get_memuse(unsigned pid)
{
	proc_list *p;
	long memuse = 0;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	memuse += STACK_SIZE;
	memuse += module_size(p->mid);
	memuse += p->memuse;

	return memuse;
}

char *get_argv(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return p->argv;
}

int get_stdio(unsigned pid, char *in, char *out)
{
	proc_list *p;

	if (in != 0)
		*in = 0;
	if (out != 0)
		*out = 0;

	if ((p = pid2procp(pid)) == 0)
		return E_NOPROC;

	if (p->d_stdin != 0 && in != 0)
		strcpy(in, p->d_stdin->device_name);
	if (p->d_stdout != 0 && out != 0)
		strcpy(out, p->d_stdout->device_name);

	return 0;
}

int set_stdio(unsigned pid, char *in, char *out)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return E_NOPROC;

	if (p->d_stdin != 0) {
		close_device(p->d_stdin);
		p->d_stdin = 0;
	}
	if (p->d_stdout != 0) {
		close_device(p->d_stdout);
		p->d_stdout = 0;
	}

	if (in != 0 && (p->d_stdin = open_device(in)) == 0)
		return E_NODEV;
	if (out != 0 && (p->d_stdout = open_device(out)) == 0)
		return E_NODEV;

	return 0;
}

int get_pflag(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return 0;

	return p->flags;
}

int set_pflag(unsigned pid, unsigned flag)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0)
		return E_NOPROC;

	p->flags = flag;
	return 0;
}

int sleep_proc(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0 || p->status != S_RUN)
		return E_NOPROC;
	if (p->flags & P_DONTKILL)
		return E_NOPROC;

	set_status(pid, S_SLEEP);
	return 0;
}

int wakeup_proc(unsigned pid)
{
	proc_list *p;

	if ((p = pid2procp(pid)) == 0 || p->status != S_SLEEP)
		return E_NOPROC;
	if (p->flags & P_DONTKILL)
		return E_NOPROC;

	set_status(pid, S_RUN);
	return 0;
}

int send_message(unsigned pid, long command, char *buf)
{
	proc_list *p;
	msgq_struct *msg;

	if ((p = pid2procp(pid)) == 0)
		return E_NOPROC;
	if ((msg = kmem_alloc(sizeof(msgq_struct))) == 0)
		return E_NOMEM;

	msg->from_pid = CurProc->pid;
	msg->command = command;
	msg->buffer = buf;
	chain_msg(p, msg);

	return 0;
}

int receive_message(unsigned *pid, long *command, char **buf)
{
	msgq_struct *msg;

	if (CurProc->msgq.head == 0)
		return E_NOMSG;

	msg = CurProc->msgq.head;
	CurProc->msgq.head = CurProc->msgq.head->next;
	CurProc->msgq.head->prev = 0;
	CurProc->msgq.nmsgs--;

	*pid = msg->from_pid;
	*command = msg->command;
	*buf = msg->buffer;
	kmem_free(msg);

	return 0;
}

int check_message(void)
{
	return CurProc->msgq.nmsgs;
}

static void chain_msg(proc_list *p, msgq_struct *msg)
{
	if (p->msgq.head == 0) {
		msg->prev = 0;
		msg->next = 0;
		p->msgq.head = msg;
		p->msgq.tail = msg;
		p->msgq.nmsgs = 1;
	} else {
		msg->prev = p->msgq.tail;
		msg->next = 0;
		p->msgq.tail->next = msg;
		p->msgq.tail = msg;
		p->msgq.nmsgs++;
	}
}

static void chain_a(proc_list *p)
{
	proc_list *q;

	if (ProcListA == 0) {
		p->next_a = 0;
		ProcListA = p;
	} else {
		if (ProcListA->pid > p->pid) {
			p->next_a = ProcListA;
			ProcListA = p;
		} else {
			for (q = ProcListA; q->next_a != 0; q = q->next_a) {
				if (q->next_a->pid > p->pid) {
					p->next_a = q->next_a;
					q->next_a = p;

					return;
				}
			}
			p->next_a = 0;
			q->next_a = p;
		}
	}
}

static void unchain_a(proc_list *p)
{
	proc_list *t;

	if (p == ProcListA) {
		ProcListA = p->next_a;
		p->next_a = 0;
	} else {
		for (t = ProcListA; t != 0; t = t->next_a) {
			if (t->next_a == p) {
				t->next_a = p->next_a;
				p->next_a = 0;
				return;
			}
		}
	}
}

static void chain_s(proc_list *p)
{
	p->next_s = ProcListS[p->status];
	ProcListS[p->status] = p;
}

static void unchain_s(proc_list *p)
{
	proc_list *t;

	if (p == ProcListS[p->status]) {
		ProcListS[p->status] = p->next_s;
		p->next_s = 0;
	} else {
		for (t = ProcListS[p->status]; t != 0; t = t->next_s) {
			if (t->next_s == p) {
				t->next_s = p->next_s;
				p->next_s = 0;
				return;
			}
		}
	}
}

static unsigned makepid(void)
{
	unsigned i;
	proc_list *t;

	for (i = 1; i < (unsigned)~0; i++) {
		for (t = ProcListA; t != 0; t = t->next_a) {
			if (t->pid == i)
				goto contin;
		}
		return i;
	contin:
		;
	}
	return 0;
}

static proc_list *pid2procp(unsigned pid)
{
	proc_list *t;

	for (t = ProcListA; t != 0; t = t->next_a) {
		if (t->pid == pid)
			return t;
	}
	return 0;
}

static unsigned *initstack(unsigned stack, unsigned ics, unsigned ids)
{
	unsigned *s;

	s = MK_FP(stack, 0);
	s = (unsigned *)((char *)s + STACK_SIZE - 2);

	*--s = 0x0200;				/* flags (IF=on) */
	*--s = ics;					/* CS */
	*--s = 0;					/* IP */
	*--s = 0;					/* BP */
	*--s = 0;					/* DI */
	*--s = 0;					/* SI */
	*--s = ids;					/* DS */
	*--s = ids;					/* ES */
	*--s = 0;					/* DX */
	*--s = 0;					/* CX */
	*--s = 0;					/* BX */
	*--s = 0;					/* AX */

	return s;
}

static void noparent(proc_list *p)
{
	proc_list *t;

	for (t = ProcListA; t != 0; t = t->next_a) {
		if (t->ppid == p->pid)
			t->ppid = 0;
	}
}

