#include "../module.h"
#include "../irc.h"
#include "../config.h"

#include <stdio.h>

static Module utopia;

static void
give_op(Module *m, char *nick, char *msg, int type)
{
	char buf[IRC_MSG_LEN];
	sprintf(buf, "Hello, %s! Welcome to %s, a socialist utopia. "
		"Don't be a meanie!", nick, conf.chan);
	irc_say(buf);
	sprintf(buf, "MODE %s +o %s", conf.chan, nick);
	irc_cmd(buf);
}

void
mod_utopia(void)
{
	utopia.name = "Utopia";
	utopia.help = "my channel is a socialist utopia. Don't be a meanie!";
	utopia.next = 0;
	utopia.f = give_op;
	utopia.s = 0;
	utopia.on = T_JOIN;
	mod_add(&utopia);
}
