#include "irc.h"
#include "config.h"
#include "module.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void help(Module *m, char **args, enum irc_type type);
static char mod_help[80];
static char *mod_invokers[2] = {"help", NULL};

static Module mod = {
	"Help",
	mod_help,
	mod_invokers,
	help,
	2,
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
help(Module *m, char **args, enum irc_type type)
{
	char buf[IRC_MSG_LEN];
	unsigned int i, j;

	do {
		for(i=0; i<IRC_MSG_LEN && (buf[i]=m->name[i]); i++);
		buf[i] = ':';
		i++;
		buf[i] = ' ';
		i++;
		j=0;
		if(m->help)
			for(; j+i<IRC_MSG_LEN && (buf[j+i]=m->help[j]); j++);
		buf[j+i] = '\0';
		irc_msg(args[0], buf);
		m = m->next;
	} while(m);
}

void mod_add(Module *m)
{
	m->next = mod.next;
	mod.next = m;
	if(m->nargs > nargsrec) nargsrec = m->nargs;
	if(nargsrec > MAX_NARGS) {
		puts("ALERT: MAXIMUM NUMBER OF ARGUMENTS SURPASSED");
		puts("Increase that number or adapt the new module");
		printf("Offending module: \"%s\"\n", m->name);
		exit(1);
	}
	printf("Added module %s\n", m->name);
}

void
mod_handle(char *msg)
{
	Module *m;
	int i, nargs=2, callmod;
	unsigned int txtlen, actlen;
	char nick[IRC_NICK_LEN];
	char txt[IRC_MSG_LEN];
	enum irc_type type;
	char *args[MAX_NARGS], *spaces[MAX_NARGS-2];

	type = irc_get_type(msg);
	if(type==T_OTHER) return;

	irc_get_nick(nick, msg);
	irc_get_text(txt, msg);

	msg[0] = '\0'; /* mark message as copied */

	args[0] = nick;
	if(nargsrec<=2)
		args[1] = txt;
	else {
		args[1] = strtok(txt, " ");
		txtlen = strlen(txt);
		for(i=2; i<nargsrec; i++) {
			args[i] = strtok(NULL, " ");
			spaces[i-2] = args[i]-1;
			if(!args[i]) {
				args[i] = args[1]+txtlen;
				break;
			}
		}
		actlen = strlen(args[i-1]);
		if(args[i-1]+actlen != args[i])
			args[i-1][actlen] = ' ';
		nargs = i;
		for(; i<nargsrec; i++)
			args[i] = args[nargs];
	}

/*	printf("nargs: %d, nargsrec: %d\n", nargs, nargsrec);
	for(i=0; i<nargsrec; i++)
		printf("args[%d]: (%d) %s\n", i, *args[i], args[i]);
	for(i=0; i<nargs-2; i++)
		printf("spaces[%d]: %ld\n", i, spaces[i]-args[1]);*/

	m = &mod;
	do {
		callmod = 1;
		if(m->on & type) {
			if(!m->invokers[0])
				callmod = 1;
			for(i=0; m->invokers[i]; i++)
				if(type==T_CHAN) {
					if(args[1][0]==conf.cmd && !strcmp(args[1]+1, m->invokers[i]))
						callmod = 1;
				} else {
					if(!strcmp(args[1], m->invokers[i]))
						callmod = 1;
				}
		}

		if(callmod) {
			for(i=0; i<m->nargs-2; i++)
				*spaces[i] = '\0';
			for(; i<nargsrec-2; i++)
				*spaces[i] = ' ';
/*			printf("Module %s\n", m->name);
			for(i=0; i<m->nargs; i++)
				printf("args[%d]: (%d) %s\n", i, *args[i], args[i]);*/
		}
		m = m->next;
	} while(m);
}

void
mod_init(void)
{
	sprintf(mod_help, "\"/msg %s help\" or \".help\" to read this help.", conf.name);
	nargsrec = mod.nargs;

	/* Register modules here */
	/*mod_utopia();
	mod_tell();
	mod_fortune();
	mod_seen();*/
}
