/*
 * filesys.h - ÉtÉ@ÉCÉãä«óù
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(FILESYS_H)
#define FILESYS_H

/* filesys.c */
int filesys_init(int files);
int getfiles(void);
int extend_filetab(int files);
int open_file(char *name, unsigned mode);
void close_file(int handle, unsigned mask);
void close_file_pid(unsigned pid);
void close_all_file(void);
unsigned read_file(void *buf, unsigned size, int handle);
unsigned write_file(void *buf, unsigned size, int handle);
long seek_file(int handle, long pos, unsigned mode);
int get_doshandle(int handle);

/* diskio.asm */
int create_file_dos(char *filename);
int open_file_dos(char *filename, unsigned mode);
void close_file_dos(int handle);
unsigned read_file_dos(void *buf, unsigned size, int handle);
unsigned write_file_dos(void *buf, unsigned size, int handle);
long seek_file_dos(int handle, long pos, unsigned mode);

#endif	/* FILESYS_H */

