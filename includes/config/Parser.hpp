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

enum	Context
{
	SERVER_CTX,
	LOCATION_CTX
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

		// validation des arguemtns
		bool						isDirectiveAllowedInContext(const std::string& name, Context ctx);
		void						validateArgumentCount(const Directive& d);
		void						validateArgumentValues(const Directive& d);

		// validation de la structure
		void						validateServers(const std::vector<ServerNode>& servers);
		void						validateServer(const ServerNode& server);
		void						validateLocation(const LocationNode& location);
		void						validateDirective(const Directive& d, Context ctx);

	public:
		Parser();
		std::vector<ServerNode>		parseFile(const std::string& filename);
};
#endif
