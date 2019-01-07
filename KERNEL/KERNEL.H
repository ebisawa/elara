/*
 * kernel.h - ÉJÅ[ÉlÉã
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(KERNEL_H)
#define KERNEL_H

#define ELARA_KERNEL

#include "defs.h"

/* kernel.c */
extern int KernelReturnCode;

void panic(char *s, ...);
void init_kernel(void);
void term_kernel(int exitcode);
char *basename(char *path);
void disp_string(char *s);

/* procsub.asm */
void interrupt far sys_entry(void);
void far xreturn_sub(unsigned xss,unsigned xsp, unsigned *s_ss,unsigned *s_sp);

/* module.c */
unsigned load_module_dlm(char *filename);
void free_module_dlm(unsigned segm);
int spawn_module_dlm(int mode, char *filename, char *argv);
int exec_module_dlm(char *filename, char *argv);
char *module_name(unsigned mid);
unsigned module_size(unsigned mid);

/* syscall.c */
extern int ProcChange;
extern int In_Kernel;
extern int LastSysCall;
extern int LastSysCall2;

void init_syscall(void);
void term_syscall(void);
void xreturn(void);

#endif	/* KERNEL_H */

