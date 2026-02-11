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
		std::vector<ServerNode> nodes = parser.parseFile(argv[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}

