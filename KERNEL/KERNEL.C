/*
 * kernel.c - カーネル
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "kernel.h"
#include "proc.h"
#include "filesys.h"
#include "memory.h"
#include "device.h"

int KernelReturnCode = K_SUCCESS;

int main(int argc, char *argv[])
{
	init_kernel();
	/*
	 * <!> プロセスを１つも作成していないので必ず P_NOWAIT
	 */
	if (spawn_module_dlm(P_NOWAIT, "init.dlm", 0) != 0) {
		disp_string("init.dlm が実行できません\r\n");
		term_kernel(K_NOINIT);
	}
	start_proc();
	term_kernel(KernelReturnCode);
}

void panic(char *s, ...)
{
	va_list vp;
	char buf[128];

	va_start(vp, s);
	vsprintf(buf, s, vp);
	disp_string("\r\nkernel panic: ");
	disp_string(buf);
	disp_string("\r\n");
	va_end(vp);
	sprintf(buf, "last systemcall: AX = %d, %d\r\n",
		LastSysCall, LastSysCall2);
	disp_string(buf);
	sprintf(buf, "process: pid %d, name '%s', stdin '%s', stdout '%s'\r\n",
		CurProc->pid,
		module_name(CurProc->mid),
		CurProc->d_stdin->device_name,
		CurProc->d_stdout->device_name);
	disp_string(buf);

	if (CurProc == 0)
		term_kernel(K_PANIC);
	else {
		KernelReturnCode = K_PANIC;
		stop_proc();
	}
}

void init_kernel(void)
{
	int files = 0;

	kmem_init();
	files = getfiles();
	if (extend_filetab(files) != 0)
		exit(K_FSERR);
	if (filesys_init(files) != 0)
		exit(K_FSERR);

	init_syscall();
}

void term_kernel(int exitcode)
{
	release_all_driver();
	close_all_file();
	term_syscall();
	exit(exitcode);
}

char *basename(char *path)
{
	char *p;

	if ((p = strrchr(path, '.')) != 0)
		*p = 0;

	if ((p = strrchr(path, '\\')) == 0)		/* 漢字を含んでるとまずいかもね */
		return path;
	else
		return p + 1;
}

void disp_string(char *s)
{
	write_file_dos(s, strlen(s), 1);	/* 1 = con */
}

