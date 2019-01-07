
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
int sendblock(unsigned char blockno, int handle);
int senddata(int handle, int far *eot);
void endtrans(void);
char far *xargv(transinfo far * far *tinfo, char far *argv);

void main(void)
{
	int i, handle;
	unsigned char far *argv, blockno = 1, eot = 0;
	long start_time;
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
	puts("-- ファイルを送信します [XMODEM (128/SUM)] --");

	for (i = 0; i < 10; i++) {
		start_time = 0;
		for (;;) {
			switch (xgetchar2()) {
			case ACK:
				if (eot) {
					tinfo->status = XYSTAT_BINARY;
					close(handle);
					endtrans();
				}
				blockno++;			/* no break */
			case NAK:
				if (sendblock(blockno, handle) == 1)
					eot = 1;

				i = 0;				/* リトライ回数リセット */
				start_time = 0;
				break;
			case CAN:
				return;
			default:
				/*
				 * XMODEMの規定では最初の NAK を１分間待つとなっているが
				 * 別に他と同様に 10秒*10 でも問題ないだろう
				 */
				if (start_time == 0)
					start_time = head->us.p.now_time;
				else {
					if (head->us.p.now_time > start_time + 10)
						goto again;
				}
			}
		}
		again:
			;
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

int sendblock(unsigned char blockno, int handle)
{
	int sum, eot = 0;

	xputchar(SOH);
	xputchar(blockno);
	xputchar((unsigned char)~blockno);

	if ((sum = senddata(handle, &eot)) == -1)
		trans_abort();

	xputchar(sum);
	if (eot)
		return 1;
	else
		return 0;
}

int senddata(int handle, int far *eot)
{
	int i, c;
	unsigned char sum = 0;

	for (i = 0; i < 128; i++) {
		if ((c = getc(handle)) == -1) {
			c = 0x1a;
			*eot = 1;
		}
		xputchar(c);
		sum += c;
	}
	return sum;
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

char far *xargv(transinfo far * far *tinfo, char far *argv)
{
	char far *p;

	if ((p = strspl(argv, " ")) == 0)
		return 0;
	*tinfo = str2farp(argv);

	return p;
}

