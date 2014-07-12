#include "../irc.h"
#include "../module.h"
#include "../utils.h"
#include "../config.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

#define OTHER_CHANNEL "#/g/ftp"

static void handle_webfriend(char **args, enum irc_type type);
static Module mod = {
	"Ftp Password",
	"when a KiwiIRC user says 'ftp' and 'pass*' or 'cred*', sajoin them to another channel and kick them.",
	NULL,
	handle_webfriend,
	2,
	T_CHAN,
	NULL
};

static regex_t ftp, pass, cred;

static void
handle_webfriend(char **args, enum irc_type type)
{
	char buf[IRC_MSG_LEN], c;
	int i;
	regmatch_t match;
	char *afterftp;

	(void) type;

	if(regexec(&ftp, args[1], 1, &match, 0))
		return;
	afterftp = args[1] + match.rm_eo;
	if(regexec(&pass, afterftp, 1, &match, 0) && regexec(&cred, afterftp, 1, &match, 0))
		return;
	/* Now send WHOIS to the server querying for the user, and if it's a webfriend time for kick */
	sprintf(buf, "WHOIS %s", args[0]);
	irc_cmd(buf);
	irc_read(buf);
	for(i=0; i<IRC_MSG_LEN && (c=buf[i]); i++)
		if(c==':' && strbeg(buf+i+1, "[www.kiwiirc.com]")) {
			sprintf(buf, "SAJOIN %s " OTHER_CHANNEL, args[0]);
			irc_cmd(buf);
			sprintf(buf, "KICK %s %s :Check the other channel.", conf.chan, args[0]);
			irc_cmd(buf);
			return;
		}
}

static void
free_regexes(void)
{
	regfree(&ftp);
	regfree(&pass);
	regfree(&cred);
}

void
mod_ftp_password(void)
{
	regcomp(&ftp, "\\<ftp\\>", REG_EXTENDED|REG_ICASE);
	regcomp(&pass, "\\<pass[a-z]*\\>", REG_EXTENDED|REG_ICASE);
	regcomp(&cred, "\\<cred[a-z]*\\>", REG_EXTENDED|REG_ICASE);
	atexit(free_regexes);
	mod_add(&mod);
}
