/*requires
<time.h>
"../irc.h"
*/

/* Number of messages stored in a LastSeen structure */
enum {
	LASTSEEN_N_MSG = 5
};

/* We'll use a linked list for storing times and last utterances. O(n) is okay
 * The latest message can always be found at msg[last_i] */
typedef struct LastSeen LastSeen;
struct LastSeen {
	char name[IRC_NICK_LEN];
	char msg[LASTSEEN_N_MSG][IRC_MSG_LEN];
	int last_i;
	time_t seen;
	LastSeen *next;
};

/* start of the chain */
extern LastSeen base;

/* find a LastSeen in the chain with the given name
 * If none is found, return NULL */
LastSeen *ls_find(char *name);

/* use this to add the module */
void mod_lastseen(void);
