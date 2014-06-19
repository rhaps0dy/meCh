#include "../module.h"
#include "../irc.h"

static Module test;

static void
mirror_msg(Module *m, char *nick, char *msg, int type)
{
	if(type==T_CHAN)
		irc_say(msg);
}

void
mod_test(void)
{
	test.name = "Test module";
	test.help = 0;
	test.next = 0;
	test.f = mirror_msg;
	test.s = 0;
	mod_add(&test);
}
