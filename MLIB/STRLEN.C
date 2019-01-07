
int strlen(char far *s)
{
	int len;

	for (len = 0; *s != 0; len++, s++)
		;

	return len;
}

