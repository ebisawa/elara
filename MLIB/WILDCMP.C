
int wildcmp(char far *str, char far *wild)
{
	while (*str != 0 && *wild != 0) {
		if (*str == *wild) {
			str++;
			wild++;
		} else {
			if (*wild != '*')
				break;
			else {
				wild++;
				while (*str != 0 && *str != *wild)
					str++;
			}
		}
	}
	return *str - *wild;
}

