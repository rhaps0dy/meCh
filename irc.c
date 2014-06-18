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
	char *name;
	char *chan;
	char *qmsg;
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

void
irc_send(char *msg)
{
	write(fd, msg, strlen(msg));
}

void
irc_msg(char *msg)
{
	char buf[1024] = "PRIVMSG ";
	strcat(buf, conf.chan);
	strcat(buf, " :");
	strcat(buf, msg);
	strcat(buf, "\n");
	write(fd, buf, strlen(buf));
}

unsigned int
irc_read(char *msg, unsigned int sz)
{
	unsigned int i;

	for(i=0; i<sz-1; i++)
		if(!read(fd, msg+i, 1) || msg[i]=='\n')
			break;
	msg[i] = '\0';
	if(i==sz-1)
		return sz;
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
