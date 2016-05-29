#include "client-features.h"
volatile sig_atomic_t running =1;

void sigint_handler (int sig)
{
	running = 0;
}

int main (int argc, char* argv[])
{
	int port;
	char* address;
	handle_args(argc, argv, &port, &address);
	jb_sethandler(sigint_handler, SIGINT);
	while (running)
	{
		run_client(port, address);
	}
	exit (EXIT_SUCCESS);
}
