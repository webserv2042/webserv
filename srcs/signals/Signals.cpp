#include "../../includes/Signals.hpp"

volatile sig_atomic_t server_running = 1; //variable qui verifie que le programme n'a pas ete interrompu par un signal

/// @brief gere le signal ctrl+C
/// @param sig sert a rien
void    handle_sigint(int sig)
{
    (void)sig;
    server_running = 0;
	std::cout << std::endl;
}

/// @brief gere les signaux (juste ctrl+C)
/// @param 
void    handle_signals(void)
{
    struct sigaction sa;
	sa.sa_handler = &handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
		throw std::runtime_error("(SIGACTION) sigaction() function failed");
}