
#include "klib.h"
#include "mlib.h"

#define TAB_WIDTH	8

static int x_getchar(void);
static int space(int n);
static int backspace(int n);
static int get_tabwidth(char far *buf, int idx, int pculm);

int gets3(char far *buf, int max, int echoback, int pculm)
{
	int i, c, clm = pculm;

	for (i = 0;; i++) {
	contin:
		if ((c = x_getchar()) == '\n')
			break;
		if (c == '\b') {
			if (i > 0) {
				i--;
				if (i > 0 && iskanji1(buf[i - 1]) && iskanji2(buf[i])) {
					i--;
					print("\b\b  \b\b");
					clm -= 2;
				} else if (buf[i] == '\t') {
					clm -= backspace(get_tabwidth(buf, i, pculm));
				} else {
					print("\b \b");
					clm--;
				}
			}
			goto contin;
		}
		if (c == '\t')
			clm += space(TAB_WIDTH - clm % TAB_WIDTH) - 1;
	
		if (c != '\t' && !isprint(c) && !iskanji1(c) && !iskanji2(c))
			goto contin;
		if (i >= max - 1)
			goto contin;

		buf[i] = c;
		clm++;
		if (echoback != 0)
			putchar(echoback);
		else {
			if (c != '\t')
				putchar(c);
		}
	}
	buf[i] = 0;
	putchar('\n');

	return i;
}

static int x_getchar(void)
{
	int c;

	while ((c = getchar()) == -1)
		;

	return c;
}

static int space(int n)
{
	int i;

	for (i = 0; i < n; i++)
		putchar(' ');

	return n;
}

static int backspace(int n)
{
	int i;

	for (i = 0; i < n; i++)
		putchar('\b');

	return n;
}

static int get_tabwidth(char far *buf, int idx, int pculm)
{
	int i, clm = pculm;

	for (i = 0;; i++) {
		if (buf[i] == 0 || i == idx)
			break;
		if (buf[i] == '\t')
			clm += TAB_WIDTH - clm % TAB_WIDTH;
		else
			clm++;
	}
	return TAB_WIDTH - clm % TAB_WIDTH;
}

