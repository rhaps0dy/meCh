#include "irc.h"
#include "config.h"
#include "utils.h"
#include "module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

static int fd;

void
irc_loop(void)
{
	char buf[IRC_MSG_LEN];
	while(1) {
		irc_read(buf);
		if(strbeg(buf, "PING")) {
			buf[1] = 'O';
			irc_cmd(buf);
		}
		else
			mod_handle(buf);
	}
}

void
irc_connect(void)
{
	struct sockaddr_in addr;
	struct hostent *serv;
	char buf[IRC_MSG_LEN];

	fd = socket(AF_INET, SOCK_STREAM/*|SOCK_NONBLOCK*/, 0);
	printf("Connecting to %s...\n", conf.host);
	serv = gethostbyname(conf.host);
	addr.sin_family = AF_INET;
	memcpy(&addr.sin_addr.s_addr, serv->h_addr_list[0], serv->h_length);
	addr.sin_port = htons(conf.port);
	if(connect(fd, (struct sockaddr *) &addr, sizeof(addr))<0) {
		puts("Connection error");
		exit(EXIT_FAILURE);
	}
	irc_read(buf);
	irc_read(buf);
	sprintf(buf, "USER %s %s %s %s", conf.name, conf.name, conf.name, conf.name);
	irc_cmd(buf);
	sprintf(buf, "NICK %s", conf.name);
	irc_cmd(buf);
	irc_read(buf);
	buf[1] = 'O';
	irc_cmd(buf);
	while(irc_read(buf)) {
		buf[strlen(conf.name)+1] = '\0';
		if(!strcmp(buf+1, conf.name)) break;
	}
	sprintf(buf, "JOIN %s", conf.chan);
	irc_cmd(buf);
	puts("Connected!");
	sprintf(buf, "Hello, I'm %s!", conf.name);
	irc_say(buf);
	while(irc_read(buf) && irc_get_type(buf)!=T_JOIN);
}

void
irc_say(char msg[IRC_MSG_LEN])
{
	char buf[IRC_MSG_LEN];

	snprintf(buf, IRC_MSG_LEN, "PRIVMSG %s :%s", conf.chan, msg);
	irc_cmd(buf);
}

void
irc_reply(char nick[IRC_NICK_LEN], char msg[IRC_MSG_LEN], enum irc_type type)
{
	char buf[IRC_MSG_LEN];

	switch(type) {
	case T_MSG:
		snprintf(buf, IRC_MSG_LEN, "PRIVMSG %s :%s", nick, msg);
		goto finish;
	case T_CHAN:
		snprintf(buf, IRC_MSG_LEN, "PRIVMSG %s :%s: %s", conf.chan, nick, msg);
		goto finish;
	default: return;
	}
finish:
	irc_cmd(buf);
}

void
irc_cmd(char msg[IRC_MSG_LEN])
{
	int i;
	for(i=0; i<IRC_MSG_LEN-2 && msg[i]; i++);
	msg[i] = '\r'; i++;
	msg[i] = '\n'; i++;
	write(fd, msg, i);
}

unsigned int
irc_read(char msg[IRC_MSG_LEN])
{
	unsigned int i;

	for(i=0; i<IRC_MSG_LEN-1; i++) {
		if(!read(fd, msg+i, 1) || msg[i]=='\n')
			break;
	}
	if(i && msg[i-1]=='\r')
		msg[i-1] = '\0';
	else
		msg[i] = '\0';
	return i;
}

void
irc_quit(void)
{
	char buf[128];
	sprintf(buf, "QUIT :%s", conf.qmsg);
	irc_cmd(buf);
	exit(EXIT_SUCCESS);
}

void
irc_get_nick(char nick[IRC_NICK_LEN], char msg[IRC_MSG_LEN])
{
	unsigned int i;

	if(msg[0]!=':' || msg[0]=='\0') {
		nick[0] = '\0';
		return;
	}
	msg++;
	for(i=0; i<IRC_NICK_LEN && *msg!='\0' && *msg!='!'; i++, msg++)
		nick[i] = *msg;
	nick[i] = '\0';
}

enum irc_type
irc_get_type(char msg[IRC_MSG_LEN])
{
	unsigned int i;

	for(i=0; ; i++) {
		if(i>=IRC_MSG_LEN) return T_OTHER;
		if(msg[i]=='\0') return T_OTHER;
		if(msg[i]==' ') break;
	}
	i++;
	if(strbeg(msg+i, "JOIN")) return T_JOIN;
	if(strbeg(msg+i, "PRIVMSG")) {
		if(msg[i+8]==conf.chan[0]) return T_CHAN;
		return T_MSG;
	}
	if(strbeg(msg+i, "MODE ")) return T_MODE;
	return T_OTHER;
}

void
irc_get_text(char txt[IRC_MSG_LEN], char msg[IRC_MSG_LEN])
{
	int i;

	for(i=1; ; i++) {
		if(i>=IRC_MSG_LEN) goto err;
		if(msg[i]=='\0') goto err;
		if(msg[i]==':') break;
	}
	i++;
	strcpy(txt, msg+i);
	return;
err:
	txt[0] = '\0';
	return;
}

void
irc_get_mode_params(char params[IRC_MSG_LEN], char msg[IRC_MSG_LEN])
{
	int i, j;

	for(i=1, j=0; j<0; j++)
		for(; ; i++) {
			if(i>=IRC_MSG_LEN) goto err;
			if(msg[i]=='\0') goto err;
			if(msg[i]==' ') break;
		}
	strcpy(params, msg+i);
	return;
err:
	params[0] = '\0';
	return;
}
