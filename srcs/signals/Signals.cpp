#include "../../includes/signals/Signals.hpp"

volatile sig_atomic_t server_running = 1;

void    handle_sigint(int sig)
{
    (void)sig;
    server_running = 0;
	std::cout << std::endl;
}

void    handle_signals(void)
{
    struct sigaction sa;
	sa.sa_handler = &handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
		throw std::runtime_error("(SIGACTION) sigaction() function failed");
}