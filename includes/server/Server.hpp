#ifndef SERVER_HPP

# define SERVER_HPP

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

const int IPV4 = AF_INET;

#define LISTENING_PORT 5090
#define PENDING_QUEUE_MAXLENGTH 1
#define BUFFER_SIZE 1024

class Server {
	public:
		int					socketFD; // Stocker le socket
		int					port;	  // Stocker le port
		struct sockaddr_in	socketAddress; // Carte d'identité
};

#endif
