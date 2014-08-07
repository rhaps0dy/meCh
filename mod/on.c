#include "../irc.h"
#include <time.h>
#include "lastseen.h"

#include "../module.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static void tell_on(char **args, enum irc_type type);

static Module on = {
	"On",
	".on <minutes> to see a list of the people who spoke in the last <minutes> minutes.",
	"on",
	tell_on,
	4,
	T_CHAN|T_MSG,
	NULL
};

/* TODO: make config file */
#define DEFAULT_MINUTES 10

static void
tell_on(char **args, enum irc_type type)
{
	LastSeen *l;
	char buf[IRC_MSG_LEN], notfirst = 0;
	int minutes;
	time_t now;
	double dt, seconds;

	if(*args[2]) {
		minutes = proper_atoi(args[2]);
		if(minutes == -1) {
			strcpy(buf, "Usage: .on [minutes]");
			goto say;
		} else if(minutes == -2) {
			strcpy(buf, "No, you won't overflow buffers with a big minutes number. You'll just cause your query to not work.");
			goto say;
		}

		sprintf(buf, "Seen on the last %s minutes: ", args[2]);
	} else {
		minutes = DEFAULT_MINUTES;
		sprintf(buf, "Seen on the last %d (default) minutes: ", DEFAULT_MINUTES);
	}

	seconds = ((double)minutes) * 60.;
	time(&now);
	l = base.next;
	while(l) {
		dt = difftime(now, l->seen);
		if(dt<seconds) {
			if(!notfirst) notfirst = 1;
			else strcat_msg(buf, ", ");
			strcat_msg(buf, l->name);
		}
		l = l->next;
	}

say:
	irc_reply(args[0], buf, type);
}

void
mod_on(void)
{
	mod_lastseen();
	mod_add(&on);
}
