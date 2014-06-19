#include "utils.h"
#include "irc.h"

int
strbeg(char *a, char *b)
{
	for(; *a==*b && *a; a++, b++);
	if(!*a || !*b) return 1;
	return 0;
}

void
strcat_msg(char *msg, char *s)
{
	unsigned int i;
	for(i=0; i<IRC_MSG_LEN && msg[i]; i++);
	for(; i<IRC_MSG_LEN-1 && (msg[i]=*s); i++, s++);
	msg[IRC_MSG_LEN-1] = '\0';
}
