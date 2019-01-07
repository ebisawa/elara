
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "xymodem.h"

#define YMODEM_BATCH

void trans_abort(void);
int xgetchar(void);
int xgetchar2(void);
void xputchar(int c);
unsigned calc_crc(unsigned c, unsigned crc);
char far *xargv(transinfo far * far *tinfo, char far *argv);
int nextfile(transinfo far *tinfo, uid_struct far *head);
int getblock(char far *buf, int blocksize, unsigned char blockno);
int block0(transinfo far *tinfo, char far *buf);

void main(void)
{
	int i, handle, eot = 0, blocksize = 1024;
	char far *argv, buf[1024];
	unsigned char blockno = 1;
	long start_time, wsize = 0;
	uid_struct far *head;
	transinfo far *tinfo;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((argv = getargv()) == 0)
		return;
	if ((argv = xargv(&tinfo, argv)) == 0)
		return;
	if ((handle = open(argv, FA_WRITE | FA_AUTODEL)) == 0)
		return;

	tinfo->status = XYSTAT_ERROR;
	puts("-- ファイルを送信してください [YMODEM] --");

	nextfile(tinfo, head);
	xputchar('C');

	for (i = 0; i < 10; i++) {
		start_time = 0;
		for (;;) {
			switch (xgetchar2()) {
			case SOH:
				blocksize = 128;	/* no break */
			case STX:
				eot = 0;
				start_time = 0;
				switch (getblock(buf, blocksize, blockno)) {
				case ACK:
					i = 0;			/* エラーカウンタのリセット */
					blockno++;		/* no break */
				case -1:
					xputchar(ACK);
					break;
				case NAK:
					xputchar(NAK);
					break;
				}
				if (tinfo->filesize - wsize < blocksize)
					write(buf, tinfo->filesize - wsize, handle);
				else
					write(buf, blocksize, handle);

				wsize += blocksize;
				blocksize = 1024;
				break;
			case CAN:
				return;
			case EOT:
				if (eot == 0) {
					xputchar(NAK);
					eot = 1;
				} else {
					close2(handle, FA_AUTODEL);
					xputchar(ACK);
#if defined(YMODEM_BATCH)
					if (nextfile(0, head) == 0)		/* null ヘッダ受信 */
						trans_abort();
#endif
					tinfo->status = XYSTAT_BINARY;
					return;
				}
				break;
			default:
				if (start_time == 0)
					start_time = head->us.p.now_time;
				else {
					if (head->us.p.now_time > start_time + 10)
						goto again;
				}
			}
		}
	again:
		xputchar(NAK);
	}
	trans_abort();
}

void trans_abort(void)
{
	xputchar(CAN);
	exit();
}

int xgetchar(void)
{
	char c;

	while (read(&c, 1, 0) == 0)
		;

	return c;
}

int xgetchar2(void)
{
	char c;

	if (read(&c, 1, 0) == 0)
		return -1;

	return c;
}

void xputchar(int c)
{
	char cc = c;

	write(&cc, 1, 0);
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

char far *xargv(transinfo far * far *tinfo, char far *argv)
{
	char far *p;

	if ((p = strspl(argv, " ")) == 0)
		return 0;
	*tinfo = str2farp(argv);

	return p;
}

int nextfile(transinfo far *tinfo, uid_struct far *head)
{
	int i, c, blocksize = 1024;
	char buf[1024];
	long start_time;

	for (i = 0; i < 10; i++) {
		xputchar('C');
		start_time = head->us.p.now_time;
		for (;;) {
			switch (xgetchar2()) {
			case SOH:
				blocksize = 128;	/* no break */
			case STX:
				if ((c = getblock(buf, blocksize, 0)) == -1)
					trans_abort();

				blocksize = 1024;
				xputchar(c);
				if (c == NAK)
					goto again;
				if (block0(tinfo, buf) != 0)
					return -2;

				return 0;
			case CAN:
				return -1;
			case EOT:
				trans_abort();
			default:
				if (head->us.p.now_time > start_time + 10)
					goto again;
			}
		}
	again:
		;
	}
	trans_abort();
}

int getblock(char far *buf, int blocksize, unsigned char blockno)
{
	int i, c;
	unsigned int crc = 0, dsd;
	unsigned char uc;

	uc = xgetchar();
	if ((unsigned char)~uc != xgetchar())
		return NAK;
	if (uc < blockno)		/* ACK が誤って NAK と解釈されたらしい */
		return -1;			/* ACK 再送 */
	if (uc > blockno)		/* 途中のブロックを受け取り損ねた */
		trans_abort();

	for (i = 0; i < blocksize; i++) {
		if ((c = xgetchar()) == -1)
			return NAK;

		buf[i] = c;
		crc = calc_crc(c, crc);
	}
	crc = calc_crc(0, crc);
	crc = calc_crc(0, crc);
	dsd = xgetchar() << 8;
	dsd |= xgetchar();

	if (crc != dsd)
		return NAK;

	return ACK;
}

int block0(transinfo far *tinfo, char far *buf)
{
	int i;
	char far *p;

	if (*buf == 0)
		return -1;
	for (i = 0; i < sizeof(tinfo->filename) - 1 && *buf != 0; i++) {
		tinfo->filename[i] = *buf;
		buf++;
	}
	tinfo->filename[i] = 0;
	if (*++buf == 0)
		return 0;

	p = strspl(buf, " ");
	tinfo->filesize = atol(buf);
	if (p != 0)
		tinfo->timestamp = atol2(p, 8);

	return 0;
}

