#include "irc.h"
#include "config.h"
#include "module.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void help(char **args, enum irc_type type);
static char *mod_invokers[2] = {"help", NULL};

static Module mod = {
	"Help",
	"$0 to read this help.",
	mod_invokers,
	help,
	3,
	T_MSG|T_CHAN,
	NULL
};

/* record for number of arguments */
static int nargsrec;

/* maximum number of arguments allowed for a module */
enum {
	MAX_NARGS = 5
};


static void
help(char **args, enum irc_type type)
{
	char aux[IRC_MSG_LEN], buf[IRC_MSG_LEN], act;
	unsigned int i, j, n, len, k;
	Module *m = &mod;
	/* i is the string index of the message we build. j is the string index of m->help.
	n is the arg number on a $<n>. k is the length of <n> in chars*/

	if(type==T_CHAN) {
		sprintf(buf, "%s: I'll send you the help in a private message.", args[0]);
		irc_say(buf);
	}

	do {
		for(i=0; i<IRC_MSG_LEN && (buf[i]=m->name[i]); i++);
		buf[i] = ':';
		i++;
		buf[i] = ' ';
		i++;
		j=0;
		if(m->help)
			for(; j+i<IRC_MSG_LEN-1 && (act=m->help[j]); j++) {
				if(act!='$') {
					buf[j+i] = act;
					continue;
				}
				n = 0;
				k = 0;
				for(j++; (act=m->help[j]) && (m->help[j]>='0' && m->help[j]<='9'); j++, k++)
					n = n*10 + (act-'0');
				j--;
				if(!act) break;
				sprintf(aux, "[\"/msg %s %s\" | \"%c%s\"]", conf.name,
					m->invokers[n], conf.cmd, m->invokers[n]);
				len = strlen(aux);
				if(j+i+len-k < IRC_MSG_LEN-1) {
					strcpy(buf+j+i-k, aux);
					i += len-k-1;
				}
			}
		if(j+i >= IRC_MSG_LEN)
			i = IRC_MSG_LEN-1;
		else i = i+j;
		buf[i] = '\0';
		irc_msg(args[0], buf);
		m = m->next;
	} while(m);
}

void mod_add(Module *m)
{
	m->next = mod.next;
	mod.next = m;
	if(*m->invokers && m->nargs<3) {
		puts("ALERT: MODULE WITH INVOKERS AND < 3 ARGUMENTS");
		goto err;
	}
	if(m->nargs > nargsrec) nargsrec = m->nargs;
	if(nargsrec > MAX_NARGS) {
		puts("ALERT: MAXIMUM NUMBER OF ARGUMENTS SURPASSED");
		puts("Increase that number or adapt the new module");
		goto err;
	}
	printf("Added module %s\n", m->name);
	return;
err:
	printf("Offending module: \"%s\"\n", m->name);
	puts("Aborting...");
	exit(1);
}

void
mod_handle(char *msg)
{
	Module *m;
	int i, nargs = 2;
	char nick[IRC_NICK_LEN];
	char txt[IRC_MSG_LEN];
	enum irc_type type;
	char *args[MAX_NARGS], *spaces[MAX_NARGS-2];

	type = irc_get_type(msg);
	if(type==T_OTHER) return;

	irc_get_nick(nick, msg);
	if(type & (T_MODE|T_KICK))
		irc_get_params(txt, msg);
	else
		irc_get_text(txt, msg);

	msg[0] = '\0'; /* mark message as copied */

	args[0] = nick;

	nargs = 1 + words(txt, args+1, spaces, nargsrec-1);

	m = &mod;
	do {
		unwords(spaces, nargs-1, m->nargs-1);
		if(m->on & type) {
			if(!m->invokers[0])
				goto call;
			for(i=0; m->invokers[i]; i++)
				if(type==T_CHAN) {
					if(args[1][0]==conf.cmd && !strcmp(args[1]+1, m->invokers[i]))
						goto call;
				} else {
					if(!strcmp(args[1], m->invokers[i]))
						goto call;
				}
		}
		goto next;

call:	m->f(args, type);
next:	m = m->next;
	} while(m);
}

void
mod_init(void)
{
	nargsrec = mod.nargs;

	/* Register modules here */
	mod_utopia();
	mod_tell();
	mod_fortune();
	mod_seen();
	mod_on();
	mod_1337();
	mod_undeop();
}
