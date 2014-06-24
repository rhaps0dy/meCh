#include "../irc.h"
#include <time.h>
#include "lastseen.h"

#include "../module.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
append_diff(char *msg, double dt)
{
	char buf[64];
	if(dt<60.)
		sprintf(buf, "%.0f seconds", dt);
	else if(dt<3600.)
		sprintf(buf, "%.0f minutes", dt/60.);
	else if(dt<3600.*24.)
		sprintf(buf, "%.0f hours", dt/3600.);
	else if(dt<3600.*24.*7.)
		sprintf(buf, "%.0f days %.0f hours", dt/(3600.*24.), (dt-trunc(dt/(3600.*24.)))/3600.);
	else
		sprintf(buf, "%.0f days", dt/(3600.*24.));
	strcat(msg, buf);
}

static void
tell_seen(Module *m, char **args, enum irc_type type)
{
	LastSeen *l;
	char buf[IRC_MSG_LEN];

	if(type==T_CHAN) {
		strcpy(buf, args[0]);
		strcat(buf, ": ");
	}
	else buf[0] = '\0';

	if(!*args[2]) {
		strcat(buf, "Usage: .seen <nick>");
		goto say;
	}
	
	if(!strcmp(args[0], args[2])) {
		strcat(buf, "You have been seen right now.");
		goto say;
	}

	l = ls_find(args[2]);
	if(!l) {
		strcat(buf, "Sorry, ");
		strcat(buf, args[2]);
		strcat(buf, " is not in the records.");
		goto say;
	}
	strcat(buf, args[2]);
	strcat(buf, " was last seen ");
	append_diff(buf, difftime(time(NULL), l->seen)); 
	strcat(buf, " ago, saying \"");
	strcat_msg(buf, l->msg);
	strcat_msg(buf, "\"");
	buf[IRC_MSG_LEN-2] = '"';
	buf[IRC_MSG_LEN-1] = '\0';
say:
	if(type==T_CHAN) irc_say(buf);
	else irc_msg(args[0], buf);
}

void
mod_seen(void)
{
	mod_lastseen();
	mod_add(&seen);
}
