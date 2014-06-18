#include "module.h"
#include <stdio.h>
#include <string.h>

static Module mod;
static char *mod_name = "Help";
static char mod_help[80];

static void
help(Module *m, char *nick, char *msg, int type)
{
	char buf[IRC_MSG_LEN];
	unsigned int i, j;

	if(type == T_CHAN) msg++; /* avoid the . */
	if(strcmp(msg, "help")) return;
	do {
		for(i=0; i<IRC_MSG_LEN && (buf[i]=m->name[i]); i++);
		buf[i] = ':';
		i++;
		buf[i] = ' ';
		i++;
		for(j=0; j+i<IRC_MSG_LEN && (buf[j+i]=m->help[j]); j++);
		buf[j+i] = '\n';
		buf[j+i+1] = '\0';
		irc_msg(nick, buf);
		m = m->next;
	} while(m);
}

void mod_add(Module *m)
{
	m->next = mod.next;
	mod.next = m;
	printf("Added module %s", m->name);
}

void
mod_handle(char *msg)
{
	Module *m;
	int type;
	char nick[IRC_NICK_LEN];
	char cmd[IRC_NICK_LEN];
	irc_get_nick(nick, msg);
	irc_get_nick(nick, msg);

	m = &mod;
	do {
		m->f(m, msg);
		m = m->next;
	} while(m);
}

void
mod_init(void)
{
	mod.name = mod_name;
	mod.help = mod_help;
	mod.f    = help;
	mod.s    = 0;
	mod.on   = T_PRIVMSG | T_CHANMSG;
	mod.next = 0;

	sprintf(mod_help, "\"/msg %s help\" or \".help\" to read this help.");

	/* Register modules here */
	mod_test();
}
