#ifndef PARSER_HPP
# define PARSER_HPP

#include <string>
#include <vector>
// Pour ouvrir et lire un fichier
#include <iostream>
#include <fstream>

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
		void	parserServerBlock();

	public:
		Parser();
		std::vector<ServerNode>	parseFile(const std::string& filename); // méthode publique pour faire le parsing
};

#endif
