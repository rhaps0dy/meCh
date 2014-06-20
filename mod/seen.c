#include "../module.h"
#include "../irc.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

static Module seen;

/* We'll use a linked list for storing times and last utterances. O(n) is okay */
typedef struct LastSeen LastSeen;
struct LastSeen {
	char name[IRC_NICK_LEN];
	char msg[IRC_MSG_LEN];
	time_t seen;
	LastSeen *next;
};

static LastSeen base = {"", "", 0, NULL};


static LastSeen *
find_time(char *name)
{
	LastSeen *l;
	l = base.next;
	while(l) {
		if(!strcmp(l->name, name))
			return l;
		l = l->next;
	}
	l = (LastSeen *) malloc(sizeof(LastSeen));
	strcpy(l->name, name);
	l->next = base.next;
	base.next = l;
	return l;
}

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
do_seen(Module *m, char *nick, char *msg, int type)
{
	LastSeen *l;
	unsigned int i;
	char buf[IRC_MSG_LEN];

	l = find_time(nick);
	strcpy(l->msg, msg);
	l->seen = time(NULL);
	if(!strbeg(msg, ".seen")) return;
	for(; *msg && *msg!=' '; msg++);
	if(!*msg) return;
	msg++;
	for(i=0; msg[i] && msg[i]!=' '; i++);
	msg[i] = '\0';
	l = find_time(msg);
	strcpy(buf, msg);
	strcat(buf, " was last seen ");
	append_diff(buf, difftime(time(NULL), l->seen)); 
	strcat(buf, " ago, saying \"");
	strcat_msg(buf, l->msg);
	strcat_msg(buf, "\"");
	msg[IRC_MSG_LEN-2] = '"';
	irc_say(msg);
}

static void
free_seen(void)
{
	LastSeen *l, *ln;
	l = base.next;
	while(l) {
		ln = l->next;
		free(l);
		l = ln;
	}
}

void
mod_seen(void)
{
	seen.name = "Seen";
	seen.help = "\".seen <nick>\" to see the last time and utterance of that nick.";
	seen.next = 0;
	seen.f = do_seen;
	seen.on = T_CHAN;
	mod_add(&seen);
	atexit(free_seen);
}
