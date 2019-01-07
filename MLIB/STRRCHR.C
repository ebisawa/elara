
char far *strrchr(char far *s, char c)
{
	char far *p = 0;

	while (*s != 0) {
		if (*s == c)
			p = s;

		s++;
	}
	return p;
}

