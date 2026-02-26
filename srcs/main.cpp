#include "../includes/server/Server.hpp"
#include "../includes/config/Parser.hpp"
#include "../includes/Signals.hpp"
#include "../includes/Webserv.hpp"


int main(int argc, char **argv)
{
	Webserv webserv;

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return (1);
	}

	try
	{
		//PARSING
		Parser parser;
		std::vector<Server> servers;
		std::vector<Config> configs = parser.parseFile(argv[1]);
		for (size_t i = 0; i < configs.size(); i++) {
			Server server(configs[i]);
			// configs[i].printConfig();
			servers.push_back(server);
		}
	
		//SIGNALS
		handle_signals();

		//INIT SERVER
		std::cout << std::endl << "Initializing servers..." << std::endl;
		webserv.setServers(servers);

		//PARTIE POLL
		std::cout << "Starting Epoll..." << std::endl;

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

