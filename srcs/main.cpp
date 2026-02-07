#include "../includes/server/Server.hpp"

int main (void)
{
	// Server	server(LISTENING_PORT);

	// try
	// {
	// 	server.init();
	// 	// std::cout << "Tentative d'accept..." << std::endl;
	// 	// int client = accept(server.socketFD, NULL, NULL);
	// 	// if (client == -1)
	// 	// 	std::cout << "Succes, cest non bloquant" << std::endl;
	// 	// else
	// 	// 	std::cout << "Fail cest bloquant" << std::endl;
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << std::endl;
	// 	return (1);
	// }

	// // Fermeture des sockets et libération des ressources
	// close(server.socketFD);

	Config configA;
	Config configB;
	configA.addServerName("mario");
	configA.setPort(5090);
	configB.setPort(5091);
	configB.addServerName("daniya");
	Server siteVitrine(configA); // port 5090
	Server siteAdmin(configB); // port 5091
	std::cout << configA.getPort() << std::endl;
	std::cout << configA.getServerName() << std::endl;
	std::cout << configB.getPort() << std::endl;
	std::cout << configB.getServerName() << std::endl;


	return (0);
}

