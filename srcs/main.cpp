#include "../includes/server/Server.hpp"
#include "../includes/Signals.hpp"
#include "../includes/Webserv.hpp"

int main (void)
{
	Webserv webserv;

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

		//PARTIE POLL
		std::cout << "Starting Epoll..." << std::endl;

		webserv.setServer(server);
		webserv.setServer(server2);

		webserv.epollLoop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what();
		std::cerr << strerror(errno) << std::endl;
		webserv.finalClean();
		return (1);
	}

	return (0);
}
