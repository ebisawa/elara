/*
 * memory.h - ÉÅÉÇÉää«óù
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(MEMORY_H)
#define MEMORY_H

#include "xstruc.h"

typedef struct mcb_struct {
	char id;
	unsigned owner;
	unsigned size;		/* paragraph */
	char dummy[3];
	union {
		char name[8];
		struct {
			char zero;
			unsigned nextsegm;
			unsigned xflag;
		} nx;
	} d;
} mcb_struct;

/* dosmem.asm */
unsigned near dosmem_alloc(unsigned parasize);
unsigned near x_dosmem_alloc(unsigned parasize);
void near dosmem_free(unsigned segm);

/* memory.c */
void kmem_init(void);
void *kmem_alloc(unsigned size);
void kmem_free(void *p);
unsigned umem_alloc(unsigned size);
void umem_free(unsigned segm, proc_list *x);
unsigned umem_alloc_x(unsigned size);
void umem_free_x(unsigned segm);
void umem_free_proc(proc_list *p);

#endif	/* MEMORY_H */

