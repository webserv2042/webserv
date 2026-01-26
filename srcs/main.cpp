#include "../includes/server/Server.hpp"

int main (void)
{
	Server	server(LISTENING_PORT);

	try
	{
		server.init();
		accept(server.socketFD, NULL, NULL);
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
