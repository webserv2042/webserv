#ifndef SERVER_HPP

# define SERVER_HPP

#include "../config/Config.hpp"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define LISTENING_PORT 8080
#define PENDING_QUEUE_MAXLENGTH SOMAXCONN
#define BUFFER_SIZE 1024

class Server {
	public:
		Server(const Config& conf);
		Server(const Server& to_copy);
        Server& operator=(const Server& to_copy);
		~Server();
		
		void init();

		// getters
		int						getSocketFD() const;
		struct sockaddr_in		getSocketAdress() const;
		int						getSocketAddressLength() const;
		const Config&			getConfig() const;

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


