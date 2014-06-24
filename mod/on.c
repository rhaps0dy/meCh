#include "../irc.h"
#include <time.h>
#include "lastseen.h"

#include "../module.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char *mod_invokers[2] = {"on", NULL};
static void tell_on(Module *m, char **args, enum irc_type type);

static Module on = {
	"On",
	".on <minutes> to see a list of the people who spoke in the last <minutes> minutes.",
	mod_invokers,
	tell_on,
	4,
	T_CHAN|T_MSG,
	NULL
};

static char *usage = "Usage: .on <minutes>";
static char min_default[64];
/* TODO: make config file */
#define DEFAULT_MINUTES 10


static int
proper_atoi(char *a)
{
	int i = 0;
	for(; *a; a++)
		if(*a<'0' || *a>'9')
			return -1;
		else
			i = i*10 + (*a-'0');
	return i;
}

static void
tell_on(Module *m, char **args, enum irc_type type)
{
	LastSeen *l;
	char buf[IRC_MSG_LEN], notfirst = 0;
	int minutes;
	time_t now;
	double dt, seconds;

	if(type==T_CHAN) {
		strcpy(buf, args[0]);
		strcat(buf, ": ");
	}
	else buf[0] = '\0';

	if(*args[2]) {
		minutes = proper_atoi(args[2]);
		if(minutes<0) {
			strcat(buf, usage);
			goto say;
		}

		strcat(buf, "Seen on the last ");
		strcat(buf, args[2]);
		strcat(buf, " minutes: ");
	} else {
		minutes = DEFAULT_MINUTES;
		strcat(buf, min_default);
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
	if(type==T_CHAN) irc_say(buf);
	else irc_msg(args[0], buf);
}

void
mod_on(void)
{
	sprintf(min_default, "Seen on the last %d (default) minutes: ", DEFAULT_MINUTES);
	mod_lastseen();
	mod_add(&on);
}
