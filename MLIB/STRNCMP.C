
int strncmp(char far *a, char far *b, unsigned n)
{
	unsigned i;

	for (i = 0; i < n - 1; i++) {
		if (*a == 0 || *b == 0 || *a != *b)
			break;

		a++;
		b++;
	}
	return *a - *b;
}

