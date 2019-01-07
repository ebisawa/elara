
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
	char buf[18];
	int i, j, tbuf[6];
	long lt;
	uid_struct far *uid;
	time_struct st;

	if ((uid = getuid(getpid())) == 0)
		return;

	print("現在の設定値 : ");
	dispdate(uid->us.u.sequencer);
	putchar(' ');
	disptime(uid->us.u.sequencer);
	putchar('\n');
	puts("変更値を入力してください。 (YY/MM/DD HH:MM:SS)");
	print("変更値       : ");
	if (gets(buf, sizeof(buf)) == 0)
		lt = uid->us.u.udat.last_msgread;
	else {
		memset(tbuf, 0, sizeof(tbuf));
		for (i = 0, j = 0; i < sizeof(buf) && j < sizeof(tbuf); i++) {
			if (buf[i] == 0)
				break;
			if (!isdigit(buf[i]))
				j++;
			else {
				tbuf[j] *= 10;
				tbuf[j] += buf[i] - '0';
			}
		}
		st.year  = tbuf[0];
		st.month = (tbuf[1] != 0) ? tbuf[1] : 1;
		st.day   = (tbuf[2] != 0) ? tbuf[2] : 1;
		st.hour  = tbuf[3];
		st.min   = tbuf[4];
		st.sec   = tbuf[5];
		lt = mktime(&st);
	}
	uid->us.u.sequencer = lt;

	print("-- ");
	dispdate(lt);
	putchar(' ');
	disptime(lt);
	puts(" に変更しました --");
}

