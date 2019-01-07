
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "xymodem.h"

void xputchar(int c);
int xgetchar(void);
unsigned calc_crc(unsigned c, unsigned crc);
void sendblock(char far *buf, int blocksize, unsigned char blockno);

void main(void)
{
	char buf[128];

	xgetchar();
	memset(buf, 0, sizeof(buf));
	sendblock(buf, sizeof(buf), 0);
}

void xputchar(int c)
{
	char cc = c;

	while (write(&cc, 1, 0) == 0)
		;
}

int xgetchar(void)
{
	char c;

	while (read(&c, 1, 0) == 0)
		;

	return c;
}

unsigned calc_crc(unsigned c, unsigned crc)
{
	int count;

	for (count = 0; count < 8; count++) {
		if (crc & 0x8000) {
			crc <<= 1;
			crc += (((c <<= 1) & 0400) != 0);
			crc ^= 0x1021;
		} else {
			crc <<= 1;
			crc += (((c <<= 1) & 0400) != 0);
		}
	}
	return crc;
}

void sendblock(char far *buf, int blocksize, unsigned char blockno)
{
	int i;
	unsigned crc = 0;

	if (blocksize == 128)
		xputchar(SOH);
	else if (blocksize == 1024)
		xputchar(STX);

	xputchar(blockno);
	xputchar((unsigned char)~blockno);

	for (i = 0; i < blocksize; i++) {
		xputchar(*buf);
		crc = calc_crc(*buf, crc);
		buf++;
	}
	crc = calc_crc(0, crc);
	crc = calc_crc(0, crc);

	xputchar((unsigned char)(crc >> 8));
	xputchar((unsigned char)(crc & 0x00ff));
}

