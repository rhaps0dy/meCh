#include "module.h"
#include <stdio.h>

static Module startMod;

static void
help(Module *m, char *msg)
{
	printf("Handling \"%s\"", msg);
}

void mod_add(Module *m)
{
	m->next = startMod.next;
	startMod.next = m;
	printf("Added module %s", m->name);
}

void
mod_handle(char *msg)
{
	Module *m;

	m = &startMod;
	do {
		m->f(m, msg);
		m = m->next;
	} while(m);
}

void
mod_init(void)
{
	startMod.name = 0;
	startMod.help = 0;
	startMod.next = 0;
	startMod.f    = help;
	startMod.s    = 0;

	/* Register modules here */
	mod_test();
}
