
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"

void cputs(char far *s, char far *color, uid_struct far *uid)
{
	if (uid->us.u.udat.term.esc & ESC_COLOR)
		print(color);

	puts(s);
	if (uid->us.u.udat.term.esc & ESC_COLOR)
		print(WHITE);
}

