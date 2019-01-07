
char far *nextelem(char far *p)
{
	while (*p++ != 0)
		;

	return p;
}

