
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "xymodem.h"

char *prtclmdl[] = {
	"tsend.dlm",
	"xsend.dlm",
	"",					/* dummy */
	"",					/* dummy */
	"ysend.dlm",
};

void getfilename(char far *filename, int max);
void set_tinfo(transinfo far *tinfo, char far *filename);

void main(void)
{
	int prtcl;
	char far *argv, filename[128], buf[128];
	uid_struct far *uid;
	transinfo tinfo;

	if ((uid = getuid(getpid())) == 0)
		return;
	if ((argv = getargv()) == 0)
		getfilename(filename, sizeof(filename));
	else
		strcpy(filename, argv);

	memset(&tinfo, 0, sizeof(tinfo));
	set_tinfo(&tinfo, filename);
	sprintf(buf, "%04X%04X %s", FP_SEG(&tinfo), FP_OFF(&tinfo), FAR(filename));
	prtcl = uid->us.u.udat.term.down_protocol;

	if (spawn(P_WAIT, prtclmdl[prtcl - 1], buf) != 0)
		puts("** このプロトコルは使えません **");
	if (tinfo.status == XYSTAT_ERROR)
		puts("** 転送失敗 **");
	else {
		if (prtcl == PRTCL_YMODEM_BATCH)
			spawn(P_WAIT, "ybend.dlm", 0);

		puts("-- 転送完了 --");
	}
}

void getfilename(char far *filename, int max)
{
	print("ファイル名 : ");
	gets(filename, max);
}

void set_tinfo(transinfo far *tinfo, char far *filename)
{
	char far *p;

	tinfo->timestamp = 0;
	tinfo->filesize = filesize(filename);

	if ((p = strrchr(filename, '\\')) != 0)
		p++;
	else
		p = filename;

	strcpy(tinfo->filename, p);
}

