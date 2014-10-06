#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>

static void mod_f(char **args, enum irc_type type);

static Module mod = {
	"Ask",
	"Answers yes or no",
	"ask",
	mod_f,
	3,
	T_CHAN|T_MSG,
	NULL
};

static void
mod_f(char **args, enum irc_type type)
{
    char c, *i;

    c=0;
    /* let it overflow */
    for(i=args[2]; *i; i++)
        c += *i;

    irc_reply(args[0], (c%2?"yes":"no"), type);
}

void
mod_ask(void)
{
	mod_add(&mod);
}
