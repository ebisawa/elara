/*
 * filesys.c - ファイルシステム
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "kernel.h"
#include "filesys.h"
#include "device.h"
#include "xstruc.h"
#include "proc.h"
#include "memory.h"

typedef struct filetab_struct {
	char filename[13];
	char fullpath[FILENAME_MAX];
	unsigned pid;
	unsigned mode;
	unsigned usingprocs;
	int doshandle;
} filetab_struct;

static int MaxFiles;
static filetab_struct *filetab;		/* filetab[0] は使用禁止 */
static char *DosFileTab;

static int search_using_file(char *name, unsigned mdoe);
static int search_free_filetab(void);
static int get_tab(char *name, unsigned mode);

int waiting;

int filesys_init(int files)
{
	MaxFiles = files - 5;		/* files をすべて使えるわけではないので */
	if ((filetab = kmem_alloc(sizeof(filetab_struct) * MaxFiles)) == 0)
		return -1;

	memset(filetab, 0, sizeof(filetab_struct) * MaxFiles);
	return 0;
}

int getfiles(void)
{
	_asm {
		mov		ah, 52h
		int		21h
		les		di, es:[bx+4]
		xor		ax,ax
	}
label:
	_asm {
		add		ax, es:[di+4]
		les		di, es:[di]
		cmp		di, -1
		jnz		label
	}
}

int extend_filetab(int files)
{
	if ((DosFileTab = kmem_alloc(files)) == 0)
		return -1;

	memset(DosFileTab, 0xff, files);
	memcpy(DosFileTab, MK_FP(_psp, 0x18), 20);
	*(unsigned far *)MK_FP(_psp, 0x32) = files;
	*(char far* far*)MK_FP(_psp, 0x34) = DosFileTab;

	return 0;
}

int open_file(char *name, unsigned mode)
{
	char *s;
	int tab, tmode;

	if ((s = strrchr(name, '\\')) == 0)
		s = name;
	else
		s++;

	tab = get_tab(s, mode & 0x00ff);
	filetab[tab].pid = CurProc->pid;
	filetab[tab].mode = mode;
	if (strlen(s) > 13)
		panic("ファイル名が長すぎる(basename) (%s)\n", s);
	strcpy(filetab[tab].filename, s);
	if (strlen(name) > FILENAME_MAX)
		panic("ファイル名が長すぎる(fillpath) (%s)\n", s);
	strcpy(filetab[tab].fullpath, name);
	mode &= 0x00ff;
	if (mode != FA_APPEND && mode != FA_RW) {
		if (mode == FA_WRITE) {
			if ((filetab[tab].doshandle = create_file_dos(name)) == 0) {
				filetab[tab].usingprocs = 0;
				return 0;
			}
		} else {
			if ((filetab[tab].doshandle = open_file_dos(name, mode)) == 0) {
				filetab[tab].usingprocs = 0;
				return 0;
			}
		}
	} else {
		tmode = (mode == FA_APPEND) ? FA_WRITE : mode;
		if ((filetab[tab].doshandle = open_file_dos(name, tmode)) == 0) {
			if ((filetab[tab].doshandle = create_file_dos(name)) == 0) {
				filetab[tab].usingprocs = 0;
				return 0;
			}
		}
		if (mode == FA_APPEND)
			seek_file_dos(filetab[tab].doshandle, 0L, FS_END);
	}
	{
		int handle, len;
		char buf[128];

		if ((handle = open_file_dos("system.log", FA_RW)) != 0) {
			seek_file_dos(handle, 0L, FS_END);
			len = sprintf(buf, "/successful, handle = %d\r\n\r\n", tab);
			write_file_dos(buf, len, handle);
			close_file_dos(handle);
		}
	}
	return tab;
}

void close_file(int handle, unsigned mask)
{
	if (filetab[handle].usingprocs == 0) {
		panic("未使用のハンドルをクローズしようとしました(%d,%s,%04X)",
			handle, filetab[handle].filename, filetab[handle].mode);
	}
	filetab[handle].mode &= ~mask;
	close_file_dos(filetab[handle].doshandle);
	if (filetab[handle].mode & FA_AUTODEL)
		remove(filetab[handle].fullpath);

	memset(&filetab[handle], 0, sizeof(filetab[handle]));
	/*
	filetab[handle].pid = 0;
	filetab[handle].filename[0] = 0;
	filetab[handle].doshandle = 0;
	filetab[handle].usingprocs = 0;
	filetab[handle].mode = 0;
	*/
	raise_event(W_FILE, 0);
	raise_event(W_FILE, handle);
}

void close_file_pid(unsigned pid)
{
	int i;

	for (i = 1; i < MaxFiles; i++) {
		if (filetab[i].pid == pid && filetab[i].usingprocs != 0)
			close_file(i, 0);
	}
}

void close_all_file(void)
{
	int i;

	for (i = 1; i < MaxFiles; i++) {
		if (filetab[i].usingprocs != 0)
			close_file(i, 0);
	}
}

unsigned read_file(void *buf, unsigned size, int handle)
{
	if (filetab[handle].usingprocs == 0) {
		panic("未使用のハンドルからの読み込み要求(%d,'%s',%04X)",
			handle, filetab[handle].filename, filetab[handle].mode);
	}
	return read_file_dos(buf, size, filetab[handle].doshandle);
}

unsigned write_file(void *buf, unsigned size, int handle)
{
	if (filetab[handle].usingprocs == 0) {
		panic("未使用のハンドルへの書き込み要求(%d,'%s',%04X)",
			handle, filetab[handle].filename, filetab[handle].mode);
	}
	return write_file_dos(buf, size, filetab[handle].doshandle);
}

long seek_file(int handle, long pos, unsigned mode)
{
	if (filetab[handle].usingprocs == 0) {
		panic("未使用のハンドルのシーク動作要求(%d,'%s',%04X)",
			handle, filetab[handle].filename, filetab[handle].mode);
	}
	return seek_file_dos(filetab[handle].doshandle, pos, mode);
}

int get_doshandle(int handle)
{
	if (filetab[handle].usingprocs == 0) {
		panic("未使用のハンドルの DOS ハンドル要求(%d,'%s',%04X)",
			handle, filetab[handle].filename, filetab[handle].mode);
	}
	return filetab[handle].doshandle;
}

static int search_using_file(char *name, unsigned mode)
{
	int i;

	for (i = 1; i < MaxFiles; i++) {
		if (strcmp(filetab[i].filename, name) == 0) {
			if (mode == FA_READ)
				continue;
			if ((filetab[i].mode & 0x00ff) == FA_READ)
				continue;
			if (filetab[i].usingprocs != 0)
				return i;
		}
	}
	return 0;
}

static int search_free_filetab(void)
{
	int i;

	for (i = 1; i < MaxFiles; i++) {
		if (filetab[i].usingprocs == 0)
			return i;
	}
	return 0;
}

static int get_tab(char *name, unsigned mode)
{
	int tab;

	for (;;) {
		if ((tab = search_using_file(name, mode)) != 0) {
			wait_event(W_FILE, tab);
			xreturn();
		} else {
			if ((tab = search_free_filetab()) == 0) {
				wait_event(W_FILE, 0);
				xreturn();
			} else {
				filetab[tab].usingprocs = 1;
				break;
			}
		}
	}
	return tab;
}

