#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void give_op(char **args, enum irc_type type);

static Module mod = {
	"Utopia",
	"this is a socialist utopia. Don't be a meanie!",
	NULL,
	give_op,
	1,
	T_JOIN,
	NULL
};

static void
give_op(char **args, enum irc_type type)
{
	char buf[IRC_MSG_LEN];

	(void) type;

	if(!strcmp(args[0], conf.name)) return;
	sprintf(buf, "Hello, %s! Welcome to %s, a socialist utopia. "
		"Don't be a meanie!", args[0], conf.chan);
	irc_say(buf);
	sprintf(buf, "MODE %s +o %s", conf.chan, args[0]);
	irc_cmd(buf);
}

void
mod_utopia(void)
{
	mod_add(&mod);
}
