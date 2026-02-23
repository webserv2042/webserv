#include "../includes/server/Server.hpp"
#include "../includes/config/Parser.hpp"

int main(int argc, char **argv)
{

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return (1);
	}

	try
	{
		Parser parser;
		std::vector<Server> servers;
		std::vector<Config> configs = parser.parseFile(argv[1]);
		for (size_t i = 0; i < configs.size(); i++) {
			Server server(configs[i]);
			configs[i].printConfig();
			servers.push_back(server);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}

