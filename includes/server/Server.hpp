#ifndef SERVER_HPP

# define SERVER_HPP

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>


const int IPV4 = AF_INET;

#define LISTENING_PORT 5090
#define PENDING_QUEUE_MAXLENGTH 1

class Server {
	public:
		int					socketFD; // Stocker le socket
		int					port;	  // Stocker le port
		struct sockaddr_in	socketAddress; // Carte d'identité
		int					socketAddressLength;
		Server(int port);
		void init();
};

#endif
