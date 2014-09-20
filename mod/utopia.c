#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>

static void give_op(char **args, enum irc_type type);

static Module mod = {
	"Utopia",
	"This is a socialist utopia! Gives op to all incoming users",
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

	sprintf(buf, "MODE %s +o %s", conf.chan, args[0]);
	irc_cmd(buf);
}

void
mod_utopia(void)
{
	mod_add(&mod);
}
