
long lmul(long number, long m)
{
	int i;
	long val = 0;

	for (i = 0; i < m; i++)
		val += number;

	return val;
}

