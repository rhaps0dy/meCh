#include "../irc.h"
#include "../module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *mod_invokers[2] = {"1337", NULL};
static void elite(char **args, enum irc_type type);

static Module leet = {
	"1337",
	"$0 t0 m4k3 y0ur m3ssag3 b3c0m3 1337.",
	mod_invokers,
	elite,
	3,
	T_CHAN|T_MSG,
	NULL
};

static void
elite(char **args, enum irc_type type)
{
	(void)args;
	(void)type;
}

void
mod_1337(void)
{
	mod_add(&leet);
}
