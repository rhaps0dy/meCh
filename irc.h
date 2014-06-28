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
void irc_say(char msg[IRC_MSG_LEN]);
/* sends a private message */
#define irc_msg(NICK, MSG) irc_reply(NICK, MSG, T_MSG)
/* sends a raw command */
void irc_cmd(char msg[IRC_MSG_LEN]);
/* either says nick: <message> on channel or sends message to nick */
void irc_reply(char nick[IRC_NICK_LEN], char msg[IRC_MSG_LEN], enum irc_type type);

/* reads one line from the server and null-terminates it.
 * the size of the msg buffer should be at least
 * IRC_MSG_LEN */
unsigned int irc_read(char msg[IRC_MSG_LEN]);

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


/* extracts the type from a message */
enum irc_type irc_get_type(char msg[IRC_MSG_LEN]);
/* extracts the nick from a message */
void irc_get_nick(char nick[IRC_NICK_LEN], char msg[IRC_MSG_LEN]);
/* extracts the text from a message */
void irc_get_text(char txt[IRC_MSG_LEN], char msg[IRC_MSG_LEN]);
