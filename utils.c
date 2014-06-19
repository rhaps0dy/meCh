#include "utils.h"

int
strbeg(char *a, char *b)
{
	for(; *a==*b && *a; a++, b++);
	if(!*a || !*b) return 1;
	return 0;
}
