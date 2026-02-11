#include "../includes/server/Server.hpp"
#include "../includes/signals/Signals.hpp"
#include "../includes/webserv/Webserv.hpp"

int main (void)
{
	try
	{
		//SIGNALS
		handle_signals();

		//INIT SERVER
		std::cout << "Initializing server..." << std::endl;

		Server	server(8080);
		Server	server2(8081);

		server.init();
		server2.init();
		// std::cout << "Tentative d'accept..." << std::endl;
		// int client = accept(server.socketFD, NULL, NULL);
		// if (client == -1)
		// 	std::cout << "Succes, cest non bloquant" << std::endl;
		// else
		// 	std::cout << "Fail cest bloquant" << std::endl;

		//PARTIE POLL
		std::cout << "Starting Epoll..." << std::endl;

		Webserv webserv;
		webserv.setServer(server);
		webserv.setServer(server2);
		webserv.epollLoop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what();
		std::cerr << strerror(errno) << std::endl; //TODO add this to every error msg
		//TODO free fds in case of error
		return (1);
	}

	return (0);
}
