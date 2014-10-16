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
	3,
	T_CHAN,
	NULL
};

/* Returns exit status of sed, or -1 if i/o to it failed */
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
		execlp("sed", "sed", "-e", cmd, NULL);
	}
	close(sedin[0]);
	close(sedout[1]);
	close(sederr[1]);
	if(write(sedin[1], in, strlen(in)) == -1) goto ioerr;
	for(i=nslashes; i<=3; i++)
		write(sedin[1], "/", 1);
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
	char buf[IRC_MSG_LEN], msg[IRC_MSG_LEN], *sed_cmd;
	int i, msg_index, is_corr_to_other;
	size_t len;
	LastSeen * ls;

	(void) type;

	len = strlen(args[1]);
	if(args[1][0] == 's' && args[1][1] == '/') {
		ls = ls_find(args[0]);
		if(!ls) {
			irc_reply(args[0], "You have never said anything!", T_CHAN);
			return;
		}
		/* can't overflow: strlen(args[1]) + strlen(args[2]) < IRC_MSG_LEN */
		if(*args[2])
			args[1][len] = ' ';
		sed_cmd = args[1];
		is_corr_to_other = 0;
	} else if(len > 0 && len-1 <= IRC_NICK_LEN && args[1][len-1] == ':' &&
		args[2][0] == 's' && args[2][1] == '/') {

		args[1][len-1] = '\0';
		ls = ls_find(args[1]);
		args[1][len-1] = ':';
		if(!ls) {
			sprintf(msg, "%s has never said anything!", args[1]);
			irc_reply(args[0], msg, T_CHAN);
			return;
		}
		sed_cmd = args[2];
		is_corr_to_other = 1;
	} else
		return;
	for(i=0, msg_index = ls->last_i; i<LASTSEEN_N_MSG; i++, msg_index--) {
		/* ls->msg is a circular buffer */
		if(msg_index < 0)
			msg_index = LASTSEEN_N_MSG-1;
		if(!ls->msg[msg_index][0])
			continue;
		if(strbeg(ls->msg[msg_index], "s/"))
			continue;
		if(substitute(buf, ls->msg[msg_index], sed_cmd))
			continue;
		if(!strcmp(ls->msg[msg_index], buf))
			continue;
		if(is_corr_to_other)
			snprintf(msg, IRC_MSG_LEN, "%s %s thinks you meant to say \"%s\"", args[1], args[0], buf);
		else
			snprintf(msg, IRC_MSG_LEN, "%s meant to say \"%s\"", args[0], buf);
		irc_say(msg);
		return;
	}
	msg_index++; /* correct for the last decrement */
	/* buf is either the last string substituted or an error message */
	if(!strcmp(ls->msg[msg_index], buf))
		sprintf(msg, "You didn't correct anything %s said recently.", (is_corr_to_other ? args[1] : "you"));
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
