#include "../irc.h"
#include "../utils.h"
#include "../module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME_1337 "mod/1337/conv.1337"
#define DEFAULT_PROB_CONVERTING 40

static void do_1337(char **args, enum irc_type type);

static Module leet = {
	"1337",
	"$0 t0 m4k3 y0ur m3ssag3 b3c0m3 1337. j00 c4n 1npu7 4 pr0bab1l17y 0f 4 "
	"ch4r4c73r b3c0m1n9 1337 w17# 4 -<probability> 4r9um3n7 4t th3 b3g1nn1n9.",
	"1337",
	do_1337,
	3,
	T_CHAN|T_MSG,
	NULL
};

enum {
	N_CHARS = 'z'-'a'+1 + 'Z'-'A'+1,
	INI_SIZE = 128
};

/* Each conv contains the number of translations a character has
and a pointer to the first of those translations. Each translation
is composed of one or several characters and ends with a null character */
typedef struct Conv Conv;
struct Conv {
	char *beg;
	int n;
};

static Conv conv_tab[N_CHARS];

static char *conv_buf;

static void
conv_buf_free(void)
{
	free(conv_buf);
}

static int
conv_ind(char c)
{
	if(c>='A' && c<='Z')
		return c-'A'+('z'-'a')+1;
	if(c>='a' && c<='z')
		return c-'a';
	return -1;
}

static int
rand_dont_convert_this_char(int prob)
{
	int n = rand()%100;
	if(n>=prob)
		return 1;
	return 0;
}

static void
print_char_translation(char c)
{
	int i, j;
	char *act;

	i = conv_ind(c);
	printf("%c: ", c);
	act = conv_tab[i].beg;
	printf("%d ", conv_tab[i].n);
	for(j=0; j<conv_tab[i].n; j++) {
		printf("%s, ", act);
		while(*act) act++;
		act++;
	}
	putchar('\n');
}

static void
do_1337(char **args, enum irc_type type)
{
	char *str, *c, buf[IRC_MSG_LEN];
	int i, j, k, n, prob, nwords;
	char *num_start[2], *num_end;

	/* check if -<prob> is passed as first argument */
	if(args[2][0] == '-') {
		nwords = words(args[2], num_start, &num_end, 2); 
		if(nwords==1)
			goto usage;
		*num_end = '\0';
		prob = proper_atoi(num_start[0]+1);
		*num_end = ' ';
		if(prob==-1)
			goto usage;
		str = num_start[1];
	} else {
		prob = DEFAULT_PROB_CONVERTING;
		str = args[2];
	}

	k = 0;
	for(; *str; str++) {
		i = conv_ind(*str);
		if(i==-1 || rand_dont_convert_this_char(prob)) {
			buf[k] = *str;
			if(k==IRC_MSG_LEN-1)
				goto end;
			k++;
			continue;
		}
		n = rand() % conv_tab[i].n;
		c = conv_tab[i].beg;
		for(j=0; j<n; j++) {
			while(*c) c++;
			c++;
		}
		while(*c) {
			buf[k] = *c;
			if(k==IRC_MSG_LEN-1)
				goto end;
			c++;
			k++;
		}
	}
end:
	buf[k] = '\0';
	irc_reply(args[0], buf, type);
	return;
usage:
	irc_reply(args[0], "Usage: .1337 -<probability> <message>", type);
}

void
mod_1337(void)
{
	FILE *f;
	int i, j, nnlines, nchars = 0;
	char c, *act;

	f = fopen(FILENAME_1337, "r");

	while(1) {
		c = getc(f);
		if(feof(f)) break;
		if((i=conv_ind(c)) == -1) goto err;
		if(getc(f)!=':') goto err;
		if(getc(f)!='\n') goto err;

		nnlines = 0;
		conv_tab[i].n = 0;
		do {
			c = getc(f);
			nchars++;
			if(c=='\n') {
				if(nnlines==0)
					conv_tab[i].n++;
				else
					nchars--;
				nnlines++;
			} else {
				nnlines = 0;
			}
		} while(nnlines<2 && !feof(f));
	}

	act = conv_buf = (char *) malloc(nchars*sizeof(char));
	CHECK_MALLOC(conv_buf);
	atexit(conv_buf_free);
	rewind(f);

	while(1) {
		c = getc(f);
		if(feof(f)) break;
		if((i=conv_ind(c)) == -1) goto err; 
		if(getc(f)!=':') goto err;
		if(getc(f)!='\n') goto err;

		conv_tab[i].beg = act;
		for(j=0; j<conv_tab[i].n; j++) {
			while((*act=getc(f)) != '\n')
				act++;
			*act = '\0';
			act++;
		}
		if(getc(f)!='\n' && !feof(f)) goto err;
	}
	for(c='a'; c<='z'; c++)
		print_char_translation(c);
	for(c='A'; c<='Z'; c++)
		print_char_translation(c);

	fclose(f);
	mod_add(&leet);
	return;
err:
	fclose(f);
	puts("Error loading module \"1337\"");
}
