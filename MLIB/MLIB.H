/*
 * mlib.h
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(MLIB_H)
#define MLIB_H

typedef void far * va_list;

#define va_start(ap, last)	((ap) = (va_list)((&(last)) + 1))

#define MK_FP(s, o)		(void far *)(((long)(s) << 16) | (o))
#define FP_SEG(p)		(unsigned)((long)p >> 16)
#define FP_OFF(p)		(unsigned)(p)

#define FAR(p)			(char far *)(p)

#define CLS			"\x1b[2J"
#define RED			"\x1b[31m"
#define GREEN		"\x1b[32m"
#define YELLOW		"\x1b[33m"
#define BLUE		"\x1b[34m"
#define MAGENTA		"\x1b[35m"
#define CYAN		"\x1b[36m"
#define WHITE		"\x1b[37m"

#define CR			"\r\n"

#define CTRL(c)		(c - '@')
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define isupper(c)	((c) >= 'A' && (c) <= 'Z')
#define islower(c)	((c) >= 'a' && (c) <= 'z')
#define isalpha(c)	(isupper(c) || islower(c))
#define isprint(c)	((c) >= 32 && (c) <= 126)
#define isspace(c)	((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define iskanji1(c) \
	(((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xef))
#define iskanji2(c) \
	((c) >= 0x40 && (c) <= 0xfc && (c) != 0x7f)

#define toupper(c)	((islower(c)) ? (c) - 'a' + 'A' : c)
#define tolower(c)	((isupper(c)) ? (c) - 'A' + 'a' : c)

#define Q_NONE	0
#define Q_YES	1
#define Q_NO	2

typedef struct {
	long quot;
	long rem;
} ldiv_t;

void near setds(void);
void near rstds(void);
int near gets(char far *buf, int max);
int near sprintf(char far *buf, char *format, ...);
int near printf(char far *format, ...);
int near strlen(char far *s);
void near strcpy(char far *dst, char far *src);
char far * near strchr(char far *s, char c);
unsigned near atou(char far *s);
int query(char far *s, int def, char far *color);
int near getchar(void);
void near puts(char far *s);
void near print(char far *s);
int near putchar(char c);
void near strcat(char far *s, char far *add);
int fgets(char far *buf, int max, int handle);
int getc(int handle);
void chop(char far *s);
int strcmp(char far *a, char far *b);
void memset(void far *s, int c, unsigned size);
int gets2(char far *buf, int max, int echoback);
void memcpy(void far *dst, void far *src, unsigned size);
long filesize(char far *filename);
ldiv_t ldiv(long number, long denom);
long atol(char far *s);
long lmul(long number, long m);
int strncmp(char far *a, char far *b, unsigned n);
void strlwr(char far *s);
unsigned getpid_n(char far *name);
char far *strspl(char far *s, char far *delim);
void strupr(char far *s);
long atol2(char far *s, int radix);
char far *strrchr(char far *s, char c);
char far *strstr(char far *a, char far *b);
void ungetch(int c);
int gets3(char far *buf, int max, int echoback, int pculm);
int wildcmp(char far *str, char far *wild);
int file_exist(char far *filename);
int stricmp(char far *a, char far *b);

#endif	/* MLIB_H */

