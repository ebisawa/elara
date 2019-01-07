
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "xymodem.h"

int getblock(unsigned char blockno, int handle);
void trans_abort(void);
int xgetchar(void);
int xgetchar2(void);
void xputchar(int c);
char far *xargv(transinfo far * far *tinfo, char far *argv);

void main(void)
{
	int i, c, handle;
	unsigned char far *argv, blockno = 1;
	long start_time;
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
	puts("-- ファイルを送信してください [XMODEM (128/SUM)] --");

	for (i = 0; i < 10; i++) {
		xputchar(NAK);
		start_time = 0;
		for (;;) {
			switch (xgetchar2()) {
			case SOH:
				start_time = 0;
				if ((c = getblock(blockno, handle)) == ACK)
					blockno++;
				else if (c == NAK)
					goto again;
				else if (c == -1)
					c = ACK;

				i = 0;			/* リトライ回数リセット */
				xputchar(c);
				break;
			case EOT:			/* 正常に受信完了 */
				close2(handle, FA_AUTODEL);
				xputchar(ACK);

				tinfo->filesize  = filesize(argv);
				tinfo->timestamp = time();
				tinfo->status    = XYSTAT_BINARY;

				return;
			case CAN:
				return;
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
		;
	}
	trans_abort();
}

int getblock(unsigned char blockno, int handle)
{
	int i, c, bno;
	unsigned char buf[128], sum = 0;

	bno = xgetchar();
	if ((unsigned char)~bno != xgetchar())
		return NAK;
	if (bno < blockno)		/* ACK が誤って NAK と解釈されたらしい */
		return -1;			/* ACK 再送 */
	if (bno > blockno)		/* 途中のブロックを受け取り損ねた */
		trans_abort();

	for (i = 0; i < 128; i++) {
		if ((c = xgetchar()) == -1)
			return NAK;

		buf[i] = c;
		sum += c;
	}
	if (xgetchar() != sum)
		return NAK;
	if (write(buf, 128, handle) != 128)
		trans_abort();

	return ACK;
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

char far *xargv(transinfo far * far *tinfo, char far *argv)
{
	char far *p;

	if ((p = strspl(argv, " ")) == 0)
		return 0;
	*tinfo = str2farp(argv);

	return p;
}

