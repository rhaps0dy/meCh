#include "../irc.h"
#include "../module.h"
#include "../config.h"
#include "../utils.h"

#include <time.h>
#include "lastseen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void mod_function(char **args, enum irc_type type);

static Module mod = {
	"Sed",
	"Use a sed(1) command to correct your previous sentence",
	NULL,
	mod_function,
	2,
	T_CHAN,
	NULL
};

static int
substitute(char out[IRC_MSG_LEN], char *in, char *cmd)
{
	int i, nslashes, prevrevslash;
	int status, sedin[2], sedout[2], sederr[2];
	ssize_t outlen;

	nslashes = 1;
	prevrevslash = 0;
	for(i=2; cmd[i]; i++) {
		if(cmd[i]=='/' && !prevrevslash) nslashes++;
		if(cmd[i]=='\\') prevrevslash = 1;
		else prevrevslash = 0;
	}

	if(pipe(sedin)  == -1) goto pipeerr;
	if(pipe(sedout) == -1) goto pipeerr;
	if(pipe(sederr) == -1) goto pipeerr;
	if(!fork()) {
		dup2(sedin[0], 0);
		dup2(sedout[1], 1);
		dup2(sederr[1], 2);
		close(sedin[1]);
		close(sedout[0]);
		close(sederr[0]);
		if(nslashes == 2) {
			sprintf(out, "%s/", cmd);
			cmd = out;
		}
		execlp("sed", "sed", "-e", cmd, NULL);
	}
	close(sedin[0]);
	close(sedout[1]);
	close(sederr[1]);
	if(write(sedin[1], in, strlen(in)) == -1) goto ioerr;
	close(sedin[1]);
	wait(&status);
	if(status)
		outlen = read(sederr[0], out, IRC_MSG_LEN);
	else
		outlen = read(sedout[0], out, IRC_MSG_LEN);
	close(sedout[0]);
	close(sederr[0]);
	if(outlen == -1) goto ioerr;
	if(outlen) {
		out[outlen] = '\0';
		return status;
	}
ioerr:
	sprintf(out, "I/O from/to pipe to sed(1) failed, contact %s.", conf.owner);
	return -1;
pipeerr:
	sprintf(out, "Opening pipe to sed(1) failed, contact %s.", conf.owner);
	return -1;
}

static void
mod_function(char **args, enum irc_type type)
{
	char buf[IRC_MSG_LEN], msg[IRC_MSG_LEN];
	int ret;
	LastSeen * ls;

	(void) type;

	if(!strbeg(args[1], "s/")) return;
	ls = ls_find(args[0]);
	if(!ls) {
		irc_reply(args[0], "You have never said anything!", T_CHAN);
		return;
	}
	ret = substitute(buf, ls->msg, args[1]);
	if(!ret) {
		snprintf(msg, IRC_MSG_LEN, "%s meant to say \"%s\"", args[0], buf);
		irc_say(msg);
		return;
	}
	irc_reply(args[0], buf, T_CHAN);
}

void
mod_sed(void)
{
	int status;
	int sedin[2], sedout[2];
	char buf[7];

	if(pipe(sedin) == -1) goto error;
	if(pipe(sedout) == -1) goto error;
	if(!fork()) {
		dup2(sedin[0], 0);
		dup2(sedout[1], 1);
		close(sedin[1]);
		close(sedout[0]);
		execlp("sed", "sed", "-e", "s/a/b/g", NULL);
		exit(1); /* should not be reachable */
	}
	close(sedin[0]);
	close(sedout[1]);
	if(write(sedin[1], "aaaaaa", 6) == -1) goto error;
	close(sedin[1]);
	wait(&status);
	printf("\"sed\" exit status: %d\n", status);
	if(status) goto error;
	if(read(sedout[0], buf, 6) == -1) goto error;
	close(sedout[0]);
	buf[6] = '\0';
	printf("\"sed\" output: %s\n", buf);
	if(strcmp(buf, "bbbbbb"))
		goto error;
	mod_add(&mod);
	return;
error:
	puts("Error loading the \"Sed\" module, will continue without it");
}
