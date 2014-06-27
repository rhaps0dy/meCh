enum {
	IRC_NICK_LEN = 32,
	IRC_CHAN_LEN = 64,
	IRC_MSG_LEN = 700
};

/* these flags denote a type of message, not fully coherent with the protocol */
enum irc_type {
	T_OTHER = 0, /* anything that doesn't fit the others */
	T_JOIN = 1,  /* an user joins the channel */
	T_CHAN = 2,  /* someone sends a message to the channel */
	T_MSG = 4    /* someone sends meCh a private message */
};

/* opens the connection with an IRC server to the
 * configured channel with the configured name */
void irc_connect(void);

/* the main loop that processes input from the server */
void irc_loop(void);

/* msg and nick must be null-terminated */
/* says this to the channel */
void irc_say(char *msg);
/* sends a private message */
void irc_msg(char *nick, char *msg);
/* sends a raw command */
void irc_cmd(char *msg);
/* either says nick: <message> on channel or sends message to nick */

/* reads one line from the server and null-terminates it.
 * the size of the msg buffer should be at least
 * IRC_MSG_LEN */
unsigned int irc_read(char *msg);

/* quits from the server sending the configured message */
void irc_quit(void);


/* msg should be a char array of size IRC_MSG_LEN at least.
 * it also should be null-terminated, for example as produced
 * by irc_read.
 *
 * nick should be at least IRC_NICK_LEN in size
 *
 * txt should be IRC_MSG_LEN too
 */

/* extracts the nick, type and text from a message */
enum irc_type irc_get(char *nick, char *txt, char *msg);
