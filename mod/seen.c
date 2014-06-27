#include "../irc.h"
#include <time.h>
#include "lastseen.h"

#include "../module.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

static char *mod_invokers[2] = {"seen", NULL};
static void tell_seen(Module *m, char **args, enum irc_type type);

static Module seen = {
	"Seen",
	".seen <nick> to see last time <nick> spoke, and what was said.",
	mod_invokers,
	tell_seen,
	4,
	T_CHAN|T_MSG,
	NULL
};

static void
print_diff(char buf[64], double dt)
{
	if(dt<60.)
		sprintf(buf, "%.0f seconds", dt);
	else if(dt<3600.)
		sprintf(buf, "%.0f minutes", dt/60.);
	else if(dt<3600.*24.)
		sprintf(buf, "%.0f hours", dt/3600.);
	else if(dt<3600.*24.*7.)
		sprintf(buf, "%.0f days %.0f hours", dt/(3600.*24.), (dt-floor(dt/(3600.*24.)))/3600.);
	else
		sprintf(buf, "%.0f days", dt/(3600.*24.));
}

static void
tell_seen(Module *m, char **args, enum irc_type type)
{
	LastSeen *l;
	char buf[IRC_MSG_LEN], time_str[64];

	if(!*args[2]) {
		strcpy(buf, "Usage: .seen <nick>");
		goto say;
	}
	
	if(!strcasecmp(args[0], args[2])) {
		strcpy(buf, "You have been seen right now.");
		goto say;
	}

	l = ls_find(args[2]);
	if(!l) {
		sprintf(buf, "Sorry, %s is not in the records.", args[2]);
		goto say;
	}

	print_diff(time_str, difftime(time(NULL), l->seen));
	snprintf(buf, IRC_MSG_LEN-buflen, "%s was last seen %s ago, saying \"%s\"",
		l->name, time_str, l->msg);

say:
	irc_reply(args[0], buf, type);
}

void
mod_seen(void)
{
	mod_lastseen();
	mod_add(&seen);
}
