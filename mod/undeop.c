#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdlib.h>
#include <stdio.h>

static void restore_op(char **args, enum irc_type type);
static Module mod = {
	"Undeop",
	"return channel operator and call the operator-taker \"meanie\".",
	NULL,
	restore_op,
	4,
	T_MODE,
	NULL
};

static void
restore_op(char **args, enum irc_type type)
{
	char buf[IRC_MSG_LEN];

	(void) type;

	if(args[1][0]=='-' && M_C_OP&irc_chan_modes(args[1])) {
		irc_reply(args[0], "Meanie!", T_CHAN);
		sprintf(buf, "MODE %s +o %s", conf.chan, args[2]);
		irc_cmd(buf);
	}
}

void
mod_undeop(void)
{
	mod_add(&mod);
}
