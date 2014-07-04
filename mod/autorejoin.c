#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdlib.h>
#include <stdio.h>

static void rejoin(char **args, enum irc_type type);
static Module mod = {
	"Autorejoin",
	"automatically rejoins the channel after being kicked, and whines about it.",
	conf.name,
	rejoin,
	3,
	T_KICK,
	NULL
};

static void
rejoin(char **args, enum irc_type type)
{
	char buf[IRC_CHAN_LEN+6];

	(void) type;

	sprintf(buf, "JOIN %s", conf.chan);
	irc_cmd(buf);
	irc_reply(args[0], "Why would you kick me? ;_;", T_CHAN);
}

void
mod_autorejoin(void)
{
	mod_add(&mod);
}
