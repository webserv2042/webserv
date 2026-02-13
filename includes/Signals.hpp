#ifndef SIGNALS_HPP

# define SIGNALS_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <signal.h>

extern volatile sig_atomic_t server_running;

void    handle_signals(void);
void    handle_sigint(int sig);

#endif