#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void thank(char **args, enum irc_type type);

static void
thank(char **args, enum irc_type type)
{
        char buf[IRC_MSG_LEN];

        (void) type;

        if(strcmp("mode/#2d [+o meCh] by *", input)) return;
        sprintf(buf, "Thank you");
        irc_say(buf);
}

void
mod_thanks(void)
{
        mod_add(&mod);
}
