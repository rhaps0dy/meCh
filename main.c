#include "irc.h"
#include "module.h"

int
main()
{
	char buf[128];

	irc_connect();
	mod_init();
	while(1) {
		irc_read(buf, 128);
		if(buf[0]=='Q' && buf[1]=='U') break;
		mod_handle(buf);
	}
	irc_quit();
	return EXIT_FAILURE; /* not reachable */
}
