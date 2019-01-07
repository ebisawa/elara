/*
 * memory.c - ƒƒ‚ƒŠŠÇ—
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdlib.h>
#include <dos.h>
#include "kernel.h"
#include "memory.h"
#include "proc.h"
#include "xstruc.h"

static mcb_struct *getmcb(unsigned segm);
static void chain(unsigned segm);
static void unchain(unsigned segm, proc_list *p);

void kmem_init(void)
{
}

void *kmem_alloc(unsigned size)
{
	return malloc(size);
}

void kmem_free(void *p)
{
	free(p);
}

unsigned umem_alloc(unsigned size)
{
	unsigned segm;

	for (;;) {
		if ((segm = dosmem_alloc(PARA(size))) != 0) {
			CurProc->memuse += PARA(size) * 16;
			chain(segm);
			break;
		}
		wait_event(W_MEMORY, 0);
		xreturn();
	}
	return segm;
}

void umem_free(unsigned segm, proc_list *x)
{
	x->memuse -= getmcb(segm)->size * 16;
	unchain(segm, x);
	dosmem_free(segm);
	raise_event(W_MEMORY, 0);
}

unsigned umem_alloc_x(unsigned size)
{
	unsigned segm;
	mcb_struct *mcb;

	for (;;) {
		if ((segm = dosmem_alloc(PARA(size))) != 0) {
			mcb = getmcb(segm);
			mcb->d.nx.zero = 0;
			mcb->d.nx.nextsegm = 0;
			mcb->d.nx.xflag = 1;

			break;
		}
		wait_event(W_MEMORY, 0);
		xreturn();
	}
	return segm;
}

void umem_free_x(unsigned segm)
{
	dosmem_free(segm);
	raise_event(W_MEMORY, 0);
}

void umem_free_proc(proc_list *p)
{
	mcb_struct *mcb = getmcb(p->stack);

	while (mcb->d.nx.nextsegm != 0)
		umem_free(mcb->d.nx.nextsegm, p);
}

static mcb_struct *getmcb(unsigned segm)
{
	return MK_FP(segm - 1, 0);
}

static void chain(unsigned segm)
{
	mcb_struct *p = getmcb(CurProc->stack);
	mcb_struct *q = getmcb(segm);

	while (p->d.nx.nextsegm != 0)
		p = getmcb(p->d.nx.nextsegm);

	p->d.nx.nextsegm = segm;
	q->d.nx.nextsegm = 0;
	q->d.nx.zero = 0;
	q->d.nx.xflag = 0;
}

static void unchain(unsigned segm, proc_list *x)
{
	mcb_struct *p = getmcb(x->stack);

	while (p->d.nx.nextsegm != segm)
		p = getmcb(p->d.nx.nextsegm);

	p->d.nx.nextsegm = getmcb(segm)->d.nx.nextsegm;
}

