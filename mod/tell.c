#include "../module.h"
#include "../irc.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Module tell;
static char tell_help[IRC_MSG_LEN];
static char* usage = "Usage: .tell <nick> <message>";

/* We'll use a linked list for storing messages. O(n) is okay */
typedef struct TellMsg TellMsg;
struct TellMsg {
	char name[IRC_NICK_LEN];
	char sender[IRC_NICK_LEN];
	char msg[IRC_MSG_LEN];
	char private;
	TellMsg *next;
};

static TellMsg base = {"", "", "", 0, NULL};

static int
add_msg(char *sender, char *msg, char private)
{
	TellMsg *new;
	unsigned int i;

	new = (TellMsg *) malloc(sizeof(TellMsg));
	strcpy(new->sender, sender);
	for(; *msg && *msg!=' '; msg++);
	if(!*msg) goto err;
	msg++;
	for(i=0; i<IRC_NICK_LEN-1 && msg[i] && (new->name[i]=msg[i])!=' '; i++);
	if(i==0 || !msg[i]) goto err;
	new->name[i] = '\0';
	strcpy(new->msg, msg+i);
	new->private = private;
	new->next = base.next;
	base.next = new;
	return 0;
err:
	free(new);
	return 1;
}

static TellMsg *
get_msg(char *name)
{
	TellMsg *prevm, *m;
	prevm = &base;
	m = base.next;
	while(m) {
		if(!strcmp(m->name, name)) {
			prevm->next = m->next;
			return m;
		}
		prevm = m;
		m = m->next;
	}
	return NULL;
}

static void
do_tell(Module *m, char *nick, char *msg, int type)
{
	TellMsg *tmsg;
	char buf[IRC_MSG_LEN];

	/* check if we have any message for that nick */
	while((tmsg=get_msg(nick))) {
		buf[0] = '(';
		buf[1] = '\0';
		strcat(buf, tmsg->sender);
		strcat(buf, ")");
		strcat_msg(buf, tmsg->msg);
		if(tmsg->private)
			irc_msg(nick, buf);
		else {
			strcpy(tmsg->msg, nick);
			strcat(tmsg->msg, ": ");
			strcat_msg(tmsg->msg, buf);
			irc_say(tmsg->msg);
		}
		free(tmsg);
	}

	if(type==T_CHAN && msg[0]=='.') msg++;
	if(!strbeg(msg, "tell")) return;
	switch(type) {
	case T_CHAN:
		if(add_msg(nick, msg, 0)) {
			irc_say(usage);
			return;
		}
		sprintf(buf, "I'll pass your message to %s.", base.next->name);
		irc_say(buf);
		return;
	case T_MSG:
		if(add_msg(nick, msg, 1)) {
			irc_msg(nick, usage);
			return;
		}
		sprintf(buf, "I'll pass your message to %s privately.", base.next->name);
		irc_msg(nick, buf);
		return;
	}
}

void
mod_tell(void)
{
	tell.name = "Tell";
	tell.help = tell_help;
	sprintf(tell_help, "\".tell <nick> <message>\" to leave a public message for someone. "
		"\"/msg %s tell <nick> <message>\" will privately send the message to that nick when they "
		"say something in the channel or to me.", conf.name);
	tell.next = 0;
	tell.f = do_tell;
	tell.on = T_JOIN|T_CHAN|T_MSG;
	mod_add(&tell);
}
