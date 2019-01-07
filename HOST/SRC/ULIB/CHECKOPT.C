
#include "klib.h"
#include "mlib.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"

int checkopt(char far *s, long flag, uid_struct far *head)
{
	int i;

	for (i = 0; i < head->us.p.maxopt; i++) {
		if (strcmp(head->us.p.option[i].name, s) == 0)
			if (head->us.p.option[i].flag & flag)
				return 1;
			else
				return 0;
	}
	return 0;
}

