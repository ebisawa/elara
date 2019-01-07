
#include "mlib.h"

ldiv_t ldiv(long number, long denom)
{
	int i;
	ldiv_t r;

	for (i = 0;; i++) {
		if (number - denom < 0)
			break;

		number -= denom;
	}
	r.quot = i;
	r.rem = number;

	return r;
}

