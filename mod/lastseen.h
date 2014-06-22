/*requires
<time.h>
"../irc.h"
*/

/* We'll use a linked list for storing times and last utterances. O(n) is okay */
typedef struct LastSeen LastSeen;
struct LastSeen {
	char name[IRC_NICK_LEN];
	char msg[IRC_MSG_LEN];
	time_t seen;
	LastSeen *next;
};

/* start of the chain */
extern LastSeen base;

/* find a LastSeen in the chain with the given name */
LastSeen *ls_find(char *name);

/* use this to add the module */
void mod_lastseen(void);
