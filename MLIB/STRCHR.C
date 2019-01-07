
char far *strchr(char far *s, char c)
{
	while (*s != 0) {
		if (*s == c)
			return s;

		s++;
	}
	return 0;
}

