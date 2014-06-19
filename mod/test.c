#include "../module.h"
#include "../irc.h"

static Module test;

static void
mirror_msg(Module *m, char *nick, char *msg, int type)
{
	irc_say(msg);
}

void
mod_test(void)
{
	test.name = "Test";
	test.help = "this module repeats all that is said!";
	test.next = 0;
	test.f = mirror_msg;
	test.s = 0;
	test.on = T_CHAN;
	mod_add(&test);
}
