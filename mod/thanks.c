#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void thank(char **args, enum irc_type type);

static Module mod = {
        "thank",
        "Says thanks to anyone who gives bot op",
        NULL,
        thank,
        1,
        T_MODE,
        NULL
};

static void
thank(char **args, enum irc_type type)
{
        char buf[IRC_MSG_LEN];

        (void) type;

        if(args[1][0]=='-' && M_C_OP|irc_chan_modes(args[1])) {
                irc_reply(args[0], "thank you!", T_CHAN);
        }
}

void
mod_thanks(void)
{
        mod_add(&mod);
}
