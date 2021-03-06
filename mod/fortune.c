#include "../irc.h"
#include "../module.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void do_fortune(char **args, enum irc_type type);

static Module fortune = {
	"Fortune",
	"$0 to know your fortune",
	"fortune",
	do_fortune,
	3,
	T_CHAN|T_MSG,
	NULL
};

static void
do_fortune(char **args, enum irc_type type)
{
	int fd[2];
	int status, i;
	char buf[IRC_MSG_LEN];

	if(pipe(fd) == -1) {
		sprintf(buf, "Opening pipe to fortune(6) failed, contact %s.", conf.owner);
		irc_reply(args[0], buf, type);
		return;
	}
	if(!fork()) {
		dup2(fd[1], 1);
		close(fd[0]);
		execlp("fortune", "fortune", NULL);
	}
	close(fd[1]);
	wait(&status);
	i = read(fd[0], buf, IRC_MSG_LEN-1);
	close(fd[0]);
	buf[i] = '\0';

	for(i=0; buf[i]; i++)
		if(buf[i]=='\t' || buf[i]=='\n')
			buf[i] = ' ';

	irc_reply(args[0], buf, type);
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
	mod_add(&fortune);
}
