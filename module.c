#include "irc.h"
#include "config.h"
#include "module.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

static Module mod;
static char mod_help[80];

static void
help(Module *m, char *nick, char *msg, int type)
{
	char buf[IRC_MSG_LEN];
	unsigned int i, j;

	if(type == T_CHAN) msg++; /* avoid the . */
	if(!strbeg(msg, "help")) return;
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
		irc_msg(nick, buf);
		m = m->next;
	} while(m);
}

void mod_add(Module *m)
{
	m->next = mod.next;
	mod.next = m;
	printf("Added module %s\n", m->name);
}

void
mod_handle(char *msg)
{
	Module *m;
	char nick[IRC_NICK_LEN];
	char txt[IRC_MSG_LEN];
	enum irc_type type;

	type = irc_get_type(msg);
	if(type!=T_OTHER)
		irc_get_nick(nick, msg);
	irc_get_text(txt, msg);
	/* mark message as copied */
	msg[0] = '\0';

	m = &mod;
	do {
		if(m->on & type)
			m->f(m, nick, txt, type);
		m = m->next;
	} while(m);
}

void
mod_init(void)
{
	mod.name = "Help";
	mod.help = mod_help;
	mod.f    = help;
	mod.on   = T_MSG | T_CHAN;
	mod.next = 0;

	sprintf(mod_help, "\"/msg %s help\" or \".help\" to read this help.", conf.name);

	/* Register modules here */
	mod_utopia();
	mod_tell();
	mod_fortune();
}
