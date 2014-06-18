enum {
	IRC_NICK_LEN = 32,
	IRC_CHAN_LEN = 64,
	IRC_MSG_LEN = 700
};

/* opens the connection with an IRC server to the
 * configured channel with the configured name */
void irc_connect(void);

/* says this to the channel */
void irc_say(char *msg);
/* sends a private message */
void irc_msg(char *nick, char *msg);
/* sends a raw command */
void irc_cmd(char *msg);

/* reads one line from the server
 * max buffer size is IRC_MSG_LEN */
unsigned int irc_read(char *msg);

/* quits from the server sending the configured message */
void irc_quit(void);

/* extracts the nick from a message */
void irc_get_nick(char *nick, char *msg);
/* extracts the message type
 * returns one of T_* */
int irc_get_type(char *msg);
/* extracts text. Specify the message type in type */
void irc_get_text(char *txt, char *msg, int type);
