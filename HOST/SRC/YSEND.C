
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "xymodem.h"

void trans_abort(void);
int xgetchar(void);
int xgetchar2(void);
void xputchar(int c);
char far *xargv(transinfo far * far *tinfo, char far *argv);
unsigned calc_crc(unsigned c, unsigned crc);
void makeblock0(char far *buf, transinfo far *tinfo);
void sendblock(char far *buf, int blocksize, unsigned char blockno);
int waitsreq(uid_struct far *head);
void sendblock0(transinfo far *tinfo, uid_struct far *head);
void endtrans(void);

void main(void)
{
	int i, c, handle, gmode, blocksize;
	char far *argv, far *p, buf[1024];
	unsigned char blockno = 1;
	long start_time, ssize = 0;
	uid_struct far *head;
	transinfo far *tinfo;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((argv = getargv()) == 0)
		return;
	if ((argv = xargv(&tinfo, argv)) == 0)
		return;
	if ((handle = open(argv, FA_READ)) == 0)
		return;

	puts("-- ファイルを送信します [YMODEM (Batch)] --");
	sendblock0(tinfo, head);
	gmode = waitsreq(head);

	for (;;) {
		blocksize = 1024;
		memset(buf, 0x1a, blocksize);

		if (read(buf, blocksize, handle) == 0) {
			close(handle);
			tinfo->status = XYSTAT_BINARY;
			endtrans();
		}
	redo:
		sendblock(buf, blocksize, blockno);

		if (gmode != 'G') {
			switch (xgetchar()) {
			case CAN:
				return;
			case NAK:
				goto redo;
			}
		} else {
			if (xgetchar2() == CAN)
				return;
		}
		blockno++;
		ssize += blocksize;
	}
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

	while (write(&cc, 1, 0) == 0)
		;
}

char far *xargv(transinfo far * far *tinfo, char far *argv)
{
	char far *p;

	if ((p = strspl(argv, " ")) == 0)
		return 0;
	*tinfo = str2farp(argv);

	return p;
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

void makeblock0(char far *buf, transinfo far *tinfo)
{
	if (tinfo->timestamp == 0)
		sprintf(buf, "%s%c%ld", FAR(tinfo->filename), 0, tinfo->filesize);
	else {
		sprintf(buf, "%s%c%ld %lo",
			FAR(tinfo->filename), 0, tinfo->filesize, tinfo->timestamp);
	}
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

int waitsreq(uid_struct far *head)
{
	long start_time;

	start_time = head->us.p.now_time;
	for (;;) {
		switch (xgetchar2()) {
		case 'C':
			return 'C';
		case 'G':
			return 'G';
		case CAN:
			exit();
		default:
			if (head->us.p.now_time > start_time + 60)
				trans_abort();
		}
	}
}

void sendblock0(transinfo far *tinfo, uid_struct far *head)
{
	char buf[128];

	memset(buf, 0, sizeof(buf));
	makeblock0(buf, tinfo);
	if (waitsreq(head) == 'G')
		sendblock(buf, sizeof(buf), 0);
	else {
		for (;;) {
			sendblock(buf, sizeof(buf), 0);
			switch (xgetchar()) {
			case ACK:
				return;
			case CAN:
				exit();
			}
		}
	}
}

void endtrans(void)
{
	int i;

	for (i = 0; i < 10; i++) {
		xputchar(EOT);
		if (xgetchar() == ACK)
			exit();
	}
	trans_abort();
}

