#ifndef SERVER_HPP

# define SERVER_HPP


#include "../config/Config.hpp"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>


const int IPV4 = AF_INET;

#define LISTENING_PORT 8080
#define PENDING_QUEUE_MAXLENGTH 1
#define BUFFER_SIZE 1024

class Server {
	public:
		Server(const Config& conf);
		~Server();
		void init();

		// getters
		int						getSocketFD() const;
		struct sockaddr_in		getSocketAdress() const;
		int						getSocketAddressLength() const;

		// setters
		void					setSocketFD(int value);
		void					setSocketAddressLength(int value);
		void					setSocketAddress(const sockaddr_in &address);

	private:
		Config					_serverConfig;
		int						_socketFD;
		struct sockaddr_in		_socketAddress; // Carte d'identité
		int						_socketAddressLength;
};

#endif

