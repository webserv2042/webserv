#ifndef PARSER_HPP
# define PARSER_HPP

#include <string>
#include <vector>
// Pour ouvrir et lire un fichier
#include <iostream>
#include <fstream>
#include <limits>

enum	ParserState
{
	GLOBAL,
	SERVER,
	LOCATION
};

struct Directive
{
	std::string					name;
	std::vector<std::string>	arguments;
};

struct LocationNode
{
	std::string					path;
	std::vector<Directive>		directives;
};

struct ServerNode
{
	std::vector<Directive>		directives;
	std::vector<LocationNode>	locations;
};

class	Parser {
	private:
		std::vector<std::string>	tokenize(const std::string& filename);
		std::vector<ServerNode>		parserServerBlock(std::vector<std::string> tokens);

	public:
		Parser();
		std::vector<ServerNode>		parseFile(const std::string& filename);
};
#endif
