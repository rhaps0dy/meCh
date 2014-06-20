#include "../module.h"
#include "../irc.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static Module fortune;
static char fortune_help[IRC_NICK_LEN+53];

static void
do_fortune(Module *m, char *nick, char *msg, int type)
{
	int fd[2];
	int status, i, j;
	char buf[IRC_MSG_LEN];

	if(type==T_CHAN && msg[0]=='.') msg++;
	if(!strbeg(msg, "fortune")) return;
	pipe(fd);
	if(!fork()) {
		dup2(fd[1], 1);
		close(fd[0]);
		execlp("fortune", "fortune", NULL);
	}
	close(fd[1]);
	wait(&status);
	i = read(fd[0], buf, IRC_MSG_LEN);
	buf[i] = '\0';

	for(i=0; buf[i]; i++) {
		j=i;
		for(; buf[i] && buf[i]!='\n'; i++)
			if(buf[i]=='\t')
				buf[i] = ' ';
		if(buf[i]) buf[i] = '\0';
		else return;

		switch(type) {
		case T_CHAN:
			irc_say(buf+j);
			break;
		case T_MSG:
			irc_msg(nick, buf+j);
			break;
		}
	}
}

void
mod_fortune(void)
{
	int status;

	fortune.name = "Fortune";
	fortune.help = fortune_help;
	sprintf(fortune_help, "\".fortune\" or \"/msg %s fortune\" to know your fortune.", conf.name);
	fortune.next = 0;
	fortune.on = T_CHAN | T_MSG;

	/* try to execute the "fortune" program */
	if(!fork()) {
		execlp("fortune", "fortune", NULL);
		exit(1); /* should not be reachable */
	}
	wait(&status);
	printf("status %d\n", status);
	if(status) {
		puts("The \"fortune\" program cannot be found in this system. The Fortune "
			"module will not be loaded");
		return;
	}
	fortune.f = do_fortune;
	mod_add(&fortune);
}
