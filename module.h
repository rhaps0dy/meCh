/*requires
"irc.h"
*/

typedef struct Module Module;
struct Module {
	char *name;         /* module name */
	char *help;         /* help text */
	char **invokers;    /* 'command' strings that invoke the module */
/* module function. Takes the same Module; an array of strings,
 * the first one is the nick and then come the arguments, as many
 * as specified in 'nargs' and then the rest of the message, and finally
 * the type of message. */
	void (*f)(Module*, char**, enum irc_type);
	/* number of arguments the char** array in the function needs,
	 * including the 'nick', 'invoker' and 'rest of message' ones */
	unsigned int nargs;
	int on;             /* when is the module executed */
	Module *next;       /* next module in the linked list */
};

/* Init the starting module */
void mod_init(void);
/* Add a module to the chain */
void mod_add(Module *m);
/* Make the module chain handle a message */
void mod_handle(char *msg);

/* Register modules here */
void mod_test(void);
void mod_utopia(void);
void mod_tell(void);
void mod_fortune(void);
void mod_seen(void);
