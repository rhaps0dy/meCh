#include "irc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

static int fd;

typedef struct Config Config;
struct Config {
	char *host;
	int port;
	char name[MAX_NICK_LEN];
	char chan[MAX_CHAN_LEN];
	char qmsg[MAX_MSG_LEN];
} conf = {
	"irc.installgentoo.com",
	6667,
	"meCh",
	"#test",
	"baka baka"
};

void
irc_connect(void)
{
	struct sockaddr_in addr;
	struct hostent *serv;
	char buf[128];

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
	irc_read(buf, 128);
	irc_read(buf, 128);
	strcpy(buf, "USER ");
	strcat(buf, conf.name);
	strcat(buf, " ");
	strcat(buf, conf.name);
	strcat(buf, " ");
	strcat(buf, conf.name);
	strcat(buf, " ");
	strcat(buf, conf.name);
	strcat(buf, "\n");
	irc_send(buf);
	strcpy(buf, "NICK ");
	strcat(buf, conf.name);
	strcat(buf, "\n");
	irc_send(buf);
	irc_read(buf, 128);
	buf[1] = 'O';
	irc_send(buf);
	while(irc_read(buf, 128)) {
		buf[strlen(conf.name)+1] = '\0';
		if(!strcmp(buf+1, conf.name)) break;
	}
	strcpy(buf, "JOIN ");
	strcat(buf, conf.chan);
	strcat(buf, "\n");
	irc_send(buf);
	puts("Connected!");
}

static void
close_msg(char *buf, char *msg, unsigned int i) {
	unsigned int j;
	i++; buf[i] = ' ';
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
	irc_send(buf);
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

	if(!strcmp(msg+i, "JOIN")) return T_JOIN;
	if(!strcmp(msg+i, "PRIVMSG")) {
		if(msg[i+8]==conf.chan[0]) return T_CHAN;
		return T_MSG;
	}
	return T_OTHER;
}

void
irc_get_text(char *txt, char *msg)
{
	int i;

	for(i=0; ; i++) {
		if(i>=IRC_MSG_LEN) return;
		if(msg[i]=='\0') return;
		if(msg[i]==':') break;
	}
	i++;
	strcpy(txt, msg+i);
}


int
main()
{
	char buf[128];

	irc_connect();
	mod_init();
	while(1) {
		irc_read(buf, 128);
		if(buf[0]=='Q' && buf[1]=='U') break;
		mod_handle(buf);
	}
	irc_quit();
	return EXIT_FAILURE; /* not reachable */
}
