
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

void main(void)
{
	char far *argv;
	char filename[128];

	if ((argv = getargv()) == 0) {
		print("filename : ");
		if (gets(filename, sizeof(filename)) == 0)
			exit();

		argv = filename;
	}
	spawn(P_WAIT, "textedit.dlm", argv);
	if (file_exist(argv))
		puts("-- ‘‚«‚İŠ®—¹ --");
}

