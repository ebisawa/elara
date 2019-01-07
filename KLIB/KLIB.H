/*
 * klib.h
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(KLIB_H)
#define KLIB_H

#include "defs.h"

void near relcpu(void);
int near exec(char far *filename, char far *argv);
int near spawn(int mode, char far *filename, char far *argv);
unsigned near getpid(void);
unsigned near getppid(unsigned pid);
char far * near getname(unsigned pid);
unsigned near getstat(unsigned pid);
unsigned near getprio(unsigned pid);
int near setprio(unsigned pid, unsigned prio);
unsigned near getwait1(unsigned pid);
unsigned near getwait2(unsigned pid);
void far * near getstack(unsigned pid);
long near getmuse(unsigned pid);
char far * near getargv(void);
int near kill(unsigned pid);
void near exit(void);
void near disp(char far *string);
void near shutdown(int retcode);
int near adddrv(char far *filename, char far *argv);
int near deldrv(char far *drvname);
int near adddev(char far *drvname, char far *devname);
int  near deldev(char far *devname);
void far * near getuid(unsigned pid);
void near setuid(unsigned pid, void far *uid);
int near vsprintf(char far *buf, char far *format, void far *ap);
void near system(char far *buf);
unsigned near firstpid(void);
unsigned near nextpid(unsigned pid);
int near sleep(unsigned pid);
int near wakeup(unsigned pid);
void near wait(unsigned event, unsigned param);
void near raise(unsigned event, unsigned param);
void far * near malloc(unsigned size);
void near free(void far *p);
void far * near xmalloc(unsigned size);
void near xfree(void far *p);
int near open(char far *filename, unsigned mode);
void near close(int handle);
unsigned near read(void far *buf, unsigned size, int handle);
unsigned near write(void far *buf, unsigned size, int handle);
long near seek(int handle, long pos, unsigned mode);
int near ioctl(char far *device_name, unsigned command, void far *param);
int near getstdio(unsigned pid, char far *in, char far *out);
int near setstdio(unsigned pid, char far *in, char far *out);
int mkdir(char far *path);
int rmdir(char far *path);
long time(void);
void localtime(time_struct far *ts, long t);
char far *ctime(long t);
unsigned getpflag(unsigned pid);
int setpflag(unsigned pid, unsigned flag);
int fileno(int handle);
int sendmsg(unsigned to, long command, char far *buffer);
int recvmsg(unsigned far *from, long far *command, char far *far *buffer);
unsigned checkmsg(void);
int remove(char far *filename);
void close2(int handle, unsigned mask);
int procsw(int sw);
int rename(char far *oldname, char far *newname);
void iolimit(char far *device_name, unsigned pid);
long mktime(time_struct far *ts);

#endif	/* KLIB_H */

