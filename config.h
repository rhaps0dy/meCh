/*requires
"irc.h"
*/

typedef struct Config Config;
struct Config {
	char *host;
	int port;
	char name[IRC_NICK_LEN];
	char chan[IRC_CHAN_LEN];
	char qmsg[IRC_MSG_LEN];
};

extern Config conf;
