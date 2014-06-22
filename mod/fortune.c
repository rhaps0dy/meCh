#include "../irc.h"
#include "../module.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void do_fortune(Module *m, char **args, enum irc_type type);
static char *mod_invokers[2] = {"fortune", NULL};

static Module fortune = {
	"Fortune",
	"$0 to know your fortune",
	mod_invokers,
	do_fortune,
	3,
	T_CHAN|T_MSG,
	NULL
};

static void
do_fortune(Module *m, char **args, enum irc_type type)
{
	int fd[2];
	int status, i, j;
	char buf[IRC_MSG_LEN];

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

	if(type==T_CHAN)
			irc_say(buf+j);
	else
			irc_msg(args[0], buf+j);
	}
}

void
mod_fortune(void)
{
	int status;

	/* try to execute the "fortune" program */
	if(!fork()) {
		execlp("fortune", "fortune", NULL);
		exit(1); /* should not be reachable */
	}
	wait(&status);
	printf("\"fortune\" exit status: %d\n", status);
	if(status) {
		puts("The \"fortune\" program cannot be found in this system. The Fortune "
			"module will not be loaded");
		return;
	}
	fortune.f = do_fortune;
	mod_add(&fortune);
}
