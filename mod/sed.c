#include "../irc.h"
#include "lastseen.h"

#include "../module.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>

static void sed(char **args, enum irc_type type);

static Module mod = {
	"Sed",
	"use sed-like syntax to correct your previous utterance: s/pattern/substitution/. "
	"Supports & for printing the matched pattern and correcting another user's utterance "
	"with s/pat/subs/ <user>",
	NULL,
	sed,
	2,
	T_CHAN,
	NULL
};

static void
sed(char **args, enum irc_type type)
{
}

void
mod_sed(void)
{
	mod_lastseen();
	mod_add(&mod);
}
