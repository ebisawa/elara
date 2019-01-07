
void memcpy(void far *dst, void far *src, unsigned size)
{
	char far *d = dst;
	char far *s = src;

	while (size-- != 0)
		*d++ = *s++;
}

