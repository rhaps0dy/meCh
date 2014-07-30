#include "../irc.h"
#include <time.h>
#include "lastseen.h"

#include "../module.h"
#include "../utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

static void do_seen(char **args, enum irc_type type);
static Module lastseen = {
	"LastSeen",
	"core module for \"Seen\" and \"On\"",
	NULL,
	do_seen,
	2,
	T_CHAN|T_MSG,
	NULL
};

LastSeen base = {"", "", 0, NULL};

LastSeen *
ls_find(char *name)
{
	LastSeen *l;
	l = base.next;
	while(l) {
		if(!strcasecmp(l->name, name)) /* case insensitive strcmp - for win32 use stricmp */
			return l;
		l = l->next;
	}
	return NULL;
}

static LastSeen *
ls_new(char *name)
{
	LastSeen *l;
	l = (LastSeen *) malloc(sizeof(LastSeen));
	CHECK_MALLOC(l);
	strcpy(l->name, name);
	l->next = base.next;
	base.next = l;
	return l;
}

static void
do_seen(char **args, enum irc_type type)
{
	LastSeen *l;

	(void) type;

	l = ls_find(args[0]);
	if(!l) l = ls_new(args[0]);
	strcpy(l->msg, args[1]);
	l->seen = time(NULL);
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
mod_lastseen(void)
{
	static char is_loaded = 0;
	if(!is_loaded) {
		mod_add(&lastseen);
		atexit(free_seen);
		is_loaded = 1;
	}
}
