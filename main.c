#include "irc.h"
#include "module.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void handle_SIGINT(int sig)
{
	(void) sig;
	puts("Exiting...");
	irc_quit();
}

int
main()
{
	srand(time(NULL));
	irc_connect();
	signal(SIGINT, handle_SIGINT);
	mod_init();
	irc_loop();
	return 1; /* not reachable */
}
