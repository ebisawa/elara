
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../cfgdat.h"
#include "../bbsdat.h"
#include "../usrdat.h"

int cquery(char far *msg, int def, uid_struct far *uid)
{
	char far *color = 0;

	if (uid->us.u.udat.term.esc & ESC_COLOR) {
		switch (def) {
		case Q_NONE:
			color = YELLOW;
			break;
		case Q_YES:
			color = CYAN;
			break;
		case Q_NO:
			color = RED;
			break;
		}
	}
	return query(msg, def, color);
}

