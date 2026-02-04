#ifndef SERVER_HPP

# define SERVER_HPP

#include "../config/Config.hpp"

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

