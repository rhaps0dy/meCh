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

int
words(char *str, char **wds, char **spaces, int maxwds)
{
	int nwds, i;

	/* maxwds needs to be at least 1 */
	if(maxwds<1) maxwds=1;

	nwds = 0;
	while(1) {
		for(; *str==' '; str++)
			if(!*str) goto out;
		wds[nwds] = str;

		nwds++;
		if(nwds==maxwds) break;

		for(; *str!=' '; str++)
			if(!*str) goto out;
		spaces[nwds-1] = str;
	}
out:
	for(; *str; str++);
	for(i=nwds; i<maxwds; i++)
		wds[i] = str;
	return nwds;
}

void
unwords(char **spaces, int nwds, int targwds)
{
	int i;
	for(i=0; i<nwds-1 && i<targwds-1; i++)
		*spaces[i] = '\0';
	for(; i<nwds-1; i++)
		*spaces[i] = ' ';
}


int
proper_atoi(char *a)
{
	int i=0, previ=0;
	for(; *a; a++) {
		if(*a<'0' || *a>'9')
			return -1;
		i = i*10 + (*a-'0');
		if(i < previ)
			return -2;
		previ = i;
	}
	return i;
}
