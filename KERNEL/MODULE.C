/*
 * module.c - モジュール管理
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "kernel.h"
#include "module.h"
#include "proc.h"
#include "memory.h"
#include "filesys.h"

typedef struct dlm_module_list dlm_module_list;

struct dlm_module_list {
	char name[9];
	unsigned refcount;
	unsigned segm;
	unsigned size;

	dlm_module_list *prev;
	dlm_module_list *next;
};

static dlm_module_list *HeadDlmModule;
static dlm_module_list *TailDlmModule;

static dlm_module_list *dlm_search_n(char *name);
static dlm_module_list *dlm_search_s(unsigned segm);
static dlm_module_list *dlm_alloc(void);
static void dlm_free(dlm_module_list *p);

static int load_module_dlm_sub(dlm_module_list *p, char *filename);

unsigned load_module_dlm(char *filename)
{
	int i;
	char *s;
	dlm_module_list *p;

	if (filename == 0)
		return 0;
	if ((p = dlm_search_n(filename)) != 0)
		p->refcount++;
	else {
		if ((p = dlm_alloc()) == 0)
			return 0;
		if (load_module_dlm_sub(p, filename) != 0) {
			dlm_free(p);
			return 0;
		}
		p->refcount = 1;

		if ((s = strrchr(filename, '\\')) == 0)
			s = filename;
		else
			s++;

		for (i = 0; s[i] != 0 && s[i] != '.' && i < sizeof(p->name); i++)
			p->name[i] = s[i];

		p->name[i] = 0;
	}
	return p->segm;
}

static int load_module_dlm_sub(dlm_module_list *p, char *filename)
{
	int handle, i;
	common_header comhead;

	if (p == 0 || filename == 0)
		return -1;
	for (i = 0; filename[i] != '.' && filename[i] != 0; i++)
		;
	if (i >= 9)
		return -1;

	if ((handle = open_file_dos(filename, FA_READ)) == 0)
		return -1;
	if (read_file_dos(&comhead, sizeof(comhead), handle) == 0) {
		close_file_dos(handle);
		return -2;
	}
	if (comhead.type != 'M' ||
			(p->segm = dosmem_alloc(PARA(comhead.memneed))) == 0) {
		close_file_dos(handle);
		return -3;
	}
	p->size = PARA(comhead.memneed) * 16;

	memset(MK_FP(p->segm, 0), 0, p->size);	/* need for BSS */
	if (read_file_dos(MK_FP(p->segm, 0), comhead.msize, handle) == 0) {
		close_file_dos(handle);
		return -4;
	}
	close_file_dos(handle);

	return 0;
}

void free_module_dlm(unsigned segm)
{
	dlm_module_list *p;

	if ((p = dlm_search_s(segm)) == 0)
		return;
	if (--p->refcount == 0) {
		if (p->segm != 0)
			dosmem_free(p->segm);

		dlm_free(p);
	}
}

int spawn_module_dlm(int mode, char *filename, char *argv)
{
	int error;

	if (mode != P_WAIT && mode != P_NOWAIT)
		return E_PARAMERR;

	if (filename == 0)
		return E_PARAMERR;
	if ((error = create_proc(filename, argv, 0)) != 0)
		return error;
	if (mode == P_WAIT)
		wait_event(W_CHILD, npid);

	return 0;
}

int exec_module_dlm(char *filename, char *argv)
{
	int error, curpid;

	if (filename == 0)
		return E_PARAMERR;

	curpid = (CurProc != 0) ? CurProc->pid : 0;
	if ((error = create_proc(filename, argv, curpid)) != 0)
		return error;

	if (CurProc != 0)
		delete_proc(CurProc->pid, 0);	/* 親を起こさない */

	return 0;
}

char *module_name(unsigned mid)
{
	dlm_module_list *p;

	if ((p = dlm_search_s(mid)) == 0)
		return 0;

	return p->name;
}

unsigned module_size(unsigned mid)
{
	dlm_module_list *p;

	if ((p = dlm_search_s(mid)) == 0)
		return 0;

	return p->size;
}


static dlm_module_list *dlm_search_n(char *name)
{
	dlm_module_list *p;
	char buf[FILENAME_MAX];

	strcpy(buf, name);			/* これもなんとかしたいっす */
	basename(buf);

	for (p = HeadDlmModule; p != 0; p = p->next) {
		if (strcmp(p->name, buf) == 0)
			return p;
	}
	return 0;
}

static dlm_module_list *dlm_search_s(unsigned segm)
{
	dlm_module_list *p;

	for (p = HeadDlmModule; p != 0; p = p->next) {
		if (p->segm == segm)
			return p;
	}
	return 0;
}

static dlm_module_list *dlm_alloc(void)
{
	dlm_module_list *p;

	if ((p = kmem_alloc(sizeof(dlm_module_list))) == 0)
		return 0;
	if (HeadDlmModule == 0)
		HeadDlmModule = p;
	else
		TailDlmModule->next = p;

	p->prev = TailDlmModule;
	p->next = 0;			/* 0 terminate */
	TailDlmModule = p;

	return p;
}

static void dlm_free(dlm_module_list *p)
{
	if (p == HeadDlmModule)
		HeadDlmModule = p->next;
	else
		p->prev->next = p->next;

	if (p == TailDlmModule)
		TailDlmModule = p->prev;
	else
		p->next->prev = p->prev;

	kmem_free(p);
}

