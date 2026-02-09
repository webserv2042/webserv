#include "../includes/server/Server.hpp"

int main (void)
{
	Server	server(LISTENING_PORT);

	try
	{
		//PARTIE MARIO
		std::cout << "Initalizing server..." << std::endl;
		server.init();

		// std::cout << "Tentative d'accept..." << std::endl;
		// int client = accept(server.socketFD, NULL, NULL);
		// if (client == -1)
		// 	std::cout << "Succes, cest non bloquant" << std::endl;
		// else
		// 	std::cout << "Fail cest bloquant" << std::endl;

		//PARTIE POLL
		std::cout << "Starting Epoll..." << std::endl;

		server.epollLoop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	// Fermeture des sockets et libération des ressources
	close(server.socketFD);
	// close(socketFD);

	return (0);
}
