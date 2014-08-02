#include "irc.h"
#include "config.h"
#include "module.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void help(char **args, enum irc_type type);

static Module mod = {
	"Help",
	"$0 to read this help.",
	"help",
	help,
	3,
	T_MSG|T_CHAN,
	NULL
};

/* record for number of arguments */
static int nargsrec;

/* string to be used as formatter for the $0 special token in the help module */
static char S0_string[128];
static int S0_base_len;

static void
help(char **args, enum irc_type type)
{
	char buf[IRC_MSG_LEN], act;
	int i, j, with_S0_total_len;
	Module *m = &mod;
	/* i is the string index of the message we build. j is the string index of m->help. */

	if(type==T_CHAN) {
		sprintf(buf, "%s: I'll send you the help in a private message.", args[0]);
		irc_say(buf);
	}

	do {
		for(i=0; i<IRC_MSG_LEN-2 && (buf[i]=m->name[i]); i++);
		buf[i] = ':';
		i++;
		buf[i] = ' ';
		i++;
		for(j=0; i<IRC_MSG_LEN && (act=m->help[j]); j++, i++) {
			if(act!='$') {
				buf[i] = act;
				continue;
			}
			j++;
			if(!(act=m->help[j]) || act!='0') break;
			if(!m->invoker) {
				printf("Module %s doesn't have an invoker, cannot use $0 in help!\n", m->name);
				exit(EXIT_FAILURE);
			}
			with_S0_total_len = i + S0_base_len + 2*strlen(m->invoker);

			if(with_S0_total_len < IRC_MSG_LEN) {
				sprintf(buf+i, S0_string, m->invoker, m->invoker);
				i = with_S0_total_len-1;
			}
		}
		buf[i] = '\0';
		irc_msg(args[0], buf);
		m = m->next;
	} while(m);
}

void mod_add(Module *m)
{
	m->next = mod.next;
	mod.next = m;
	if(m->invoker && m->nargs<3) {
		puts("ALERT: MODULE WITH INVOKER AND < 3 ARGUMENTS");
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
	exit(EXIT_FAILURE);
}

void
mod_handle(char *msg)
{
	Module *m;
	int nargs;
	char nick[IRC_NICK_LEN], txt[IRC_MSG_LEN],
		*args[MAX_NARGS], *spaces[MAX_NARGS-2];
	enum irc_type type;

	type = irc_get_type(msg);
	if(type==T_OTHER) return;

	irc_get_nick(nick, msg);
	if(!strcmp(nick, conf.name)) return;
	if(type & (T_MODE|T_KICK))
		irc_get_params(txt, msg);
	else
		irc_get_text(txt, msg);

	/* mark message as copied, maybe useful if the bot is made multithreaded */
	msg[0] = '\0';

	args[0] = nick;

	nargs = 1 + words(txt, args+1, spaces, nargsrec-1);

	m = &mod;
	do {
		unwords(spaces, nargs-1, m->nargs-1);
		if((m->on & type) && (
			(!m->invoker) ||
			(type==T_CHAN && args[1][0]==conf.cmd && !strcmp(args[1]+1, m->invoker)) ||
			(type!=T_CHAN && !strcmp(args[1], m->invoker))
			)
		) m->f(args, type);

	m = m->next;
	} while(m);
}

void
mod_init(void)
{
	nargsrec = mod.nargs;

	snprintf(S0_string, 128, "[\"/msg %s %%s\" | \"%c%%s\"]", conf.name, conf.cmd);
	S0_base_len = strlen(S0_string)-2*2;

	/* Register modules here */
	mod_voice();
	mod_tell();
	mod_fortune();
	mod_seen();
	mod_on();
	mod_1337();
	mod_autorejoin();
	mod_sed();
}
