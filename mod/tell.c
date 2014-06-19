#include "../module.h"
#include "../irc.h"
#include "../config.h"

#include <stdio.h>

static Module tell;
static char tell_help[IRC_MSG_LEN];

static void
do_tell(Module *m, char *nick, char *msg, int type)
{
	switch(type) {
	case T_CHAN:
		irc_say("This has yet to be implemented!");
		break;
	case T_MSG:
		irc_msg(nick, "This has yet to be implemented!");
		break;
	case T_JOIN:
		break;
	}
}

void
mod_tell(void)
{
	tell.name = "Tell";
	tell.help = tell_help;
	sprintf(tell_help, "\".tell <nick> <message>\" to leave a public message for someone. "
		"\"/msg %s tell <nick> <message>\" will privately send the message to that nick when they"
		"say something in the channel or to %s", conf.name, conf.name);
	tell.next = 0;
	tell.f = do_tell;
	tell.s = 0;
	tell.on = T_JOIN|T_CHAN|T_MSG;
	mod_add(&tell);
}
