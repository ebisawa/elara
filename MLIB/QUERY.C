
#include "klib.h"
#include "mlib.h"

char *ynstr[] = { "y/n", "[y]/n", "y/[n]" };

int query(char far *s, int def, char far *color)
{
	char buf[2];

	for (;;) {
		if (color != 0)
			print(color);
		printf("%s ? (%s) : ", s, FAR(ynstr[def]));
		if (color != 0)
			print(WHITE);

		if (gets(buf, sizeof(buf)) == 0) {
			if (def != 0)
				return def;
		} else {
			if (buf[0] == 'y' || buf[0] == 'Y')
				return Q_YES;
			if (buf[0] == 'n' || buf[0] == 'N')
				return Q_NO;
		}
	}
}

