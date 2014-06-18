/* these flags control when a module is executed. They denote a type of message */
enum {
	T_JOIN = 1, /* an user joins the channel */
	T_CHAN = 2, /* someone sends a message to the channel */
	T_MSG = 4   /* someone sends meCh a private message */
};

typedef struct Module Module;
struct Module {
	char *name;                            /* module name */
	char *help;                            /* help text */
	void (*f)(Module*, char*, char*, int); /* module function */
	void *s;                               /* state, size is defined in the module */
	int on;                                /* when is the module executed */
	Module *next;                          /* next module in the linked list */
};

/* Init the starting module */
void mod_init(void);
/* Add a module to the chain */
void mod_add(Module *m);
/* Make the module chain handle a message */
void mod_handle(char *msg);
/* Register modules here */
void mod_test(void);
