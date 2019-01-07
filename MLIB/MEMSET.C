
void memset(void far *s, int c, unsigned size)
{
	unsigned i;
	char far *p = s;

	for (i = 0; i < size; i++)
		p[i] = c;
}

