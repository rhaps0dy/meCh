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
	memcpy(&addr.sin_addr.s_addr, serv->h_addr, serv->h_length);
	addr.sin_port = htons(conf.port);
	if(connect(fd, (struct sockaddr *) &addr, sizeof(addr))<0) {
		puts("Connection error");
		exit(EXIT_FAILURE);
	}
	irc_read(buf);
	irc_read(buf);
	strcpy(buf, "USER ");
	strcat(buf, conf.name);
	strcat(buf, " ");
	strcat(buf, conf.name);
	strcat(buf, " ");
	strcat(buf, conf.name);
	strcat(buf, " ");
	strcat(buf, conf.name);
	strcat(buf, "\n");
	irc_cmd(buf);
	strcpy(buf, "NICK ");
	strcat(buf, conf.name);
	strcat(buf, "\n");
	irc_cmd(buf);
	irc_read(buf);
	buf[1] = 'O';
	irc_cmd(buf);
	while(irc_read(buf)) {
		buf[strlen(conf.name)+1] = '\0';
		if(!strcmp(buf+1, conf.name)) break;
	}
	strcpy(buf, "JOIN ");
	strcat(buf, conf.chan);
	strcat(buf, "\n");
	irc_cmd(buf);
	puts("Connected!");
	sprintf(buf, "Hello, I'm %s!", conf.name);
	irc_say(buf);
	while(irc_read(buf) && irc_get_type(buf)!=T_JOIN);
}

static void
close_msg(char *buf, char *msg, unsigned int i) {
	unsigned int j;
	buf[i] = ' ';
	i++; buf[i] = ':';
	for(j=0, i++; j<IRC_MSG_LEN && msg[j]!='\0'; j++, i++)
		buf[i] = msg[j];
	i++; buf[i] = '\n';
	i++; buf[i] = '\0';
	write(fd, buf, i);
}

void
irc_say(char *msg)
{
	char buf[IRC_MSG_LEN+IRC_CHAN_LEN+12] = "PRIVMSG ";
	unsigned int i, j;

	for(j=0, i=8; j<IRC_CHAN_LEN && conf.chan[j]!='\0'; j++, i++)
		buf[i] = conf.chan[j];
	close_msg(buf, msg, i);
}

void
irc_msg(char *nick, char *msg)
{
	char buf[IRC_MSG_LEN+IRC_NICK_LEN+12] = "PRIVMSG ";
	unsigned int i, j;

	for(j=0, i=8; j<IRC_NICK_LEN && nick[j]!='\0'; j++, i++)
		buf[i] = nick[j];
	close_msg(buf, msg, i);
}

void
irc_cmd(char *msg)
{
	int i;
	for(i=0; msg[i]; i++);
	msg[i] = '\n';
	write(fd, msg, i+1);
}

unsigned int
irc_read(char *msg)
{
	unsigned int i;

	for(i=0; i<IRC_MSG_LEN-1; i++)
		if(!read(fd, msg+i, 1) || msg[i]=='\n')
			break;
	msg[i] = '\0';
	if(i==IRC_MSG_LEN-1) /* we didn't read a whole line */
		return IRC_MSG_LEN;
	return i;
}

void
irc_quit(void)
{
	char buf[128] = "QUIT :";
	strcat(buf, conf.qmsg);
	strcat(buf, "\n");
	irc_cmd(buf);
	exit(EXIT_SUCCESS);
}

void
irc_get_nick(char *nick, char *msg)
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
irc_get_type(char *msg)
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
	return T_OTHER;
}

void
irc_get_text(char *txt, char *msg)
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
