#include"../../includes/config/Parser.hpp"

Parser::Parser() {};

std::vector<ServerNode> Parser::parseFile(const std::string &filename)
{
	std::vector<ServerNode>	servers;
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Impossible d'ouvrir le fichier de configuration");
	return (servers);
}
