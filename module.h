typedef struct Module Module;
struct Module {
	char *name; 				/* module name */
	char *help;					/* help text */
	Module *next;				/* next module in the linked list */
	void (*f)(Module*, char*);	/* module function */
	void *s;					/* state, size is defined in the module */
};

/* Init the starting module */
void mod_init(void);
/* Add a module to the chain */
void mod_add(Module *m);
/* Make the module chain handle a message */
void mod_handle(char *msg);

/* Register modules here */
void mod_test(void);
