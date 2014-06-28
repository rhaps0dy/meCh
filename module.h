/*requires
"irc.h"
*/


/*Module
modules should use exactly the number of arguments they list in nargs.
If they want to use the invokers, they should have nargs>=3.
invokers are strings that make the module be ran, if they are typed as
/msg <conf.name> <invoker> or <conf.cmd><invoker>. The invokers array
must finish with a NULL, if the NULL is the only value in it the module
is ran always.
you can use $<number> in the help to insert
["/msg <conf.name> <invoker>" | "<conf.cmd><invoker>"] in the help text.
To print "$" use "$$"*/
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
void mod_on(void);
void mod_1337(void);
