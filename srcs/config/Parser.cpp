#include"../../includes/config/Parser.hpp"

Parser::Parser() {};

bool isSeparator(char c)
{
	return (c == ' ' || c == '\r' || c == '\n' || c == '{' || c == '}' || c == ';' || c == '\t');
}

bool isSingleCharToken(char c)
{
	return (c == '{' || c == '}' || c == ';');
}

/*
	Fonction qui parse le fichier et retourne un vecteur de mots
	1 - On ouvre le fichier
	2 - On lit le fichier caractère par caractère
	3 - Si on trouve un séparateur et que le mot n'est pas vide alors on ajoute le mot au vecteur
	4 - Si on trouve un caractère unique alors on ajoute le caractère au vecteur
	5 - On ferme le fichier
	6 - On retourne le vecteur
*/
std::vector<std::string> Parser::tokenize(const std::string &filename)
{
	char						c;
	std::string					currentWord;
	std::vector<std::string>	tokens;
	std::ifstream				file(filename.c_str());

	if (!file.is_open())
		throw std::runtime_error("Impossible d'ouvrir le fichier de configuration");

	while (file.get(c))
	{
		if (c == '#')
		{
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		if (isSeparator(c))
		{
			if (!currentWord.empty())
			{
				tokens.push_back(currentWord);
				currentWord.clear();
			}
			if (isSingleCharToken(c))
				tokens.push_back(std::string(1, c));
		}
		else
			currentWord += c;
	}
	if (!currentWord.empty())
		tokens.push_back(currentWord);
	return (tokens);
}

bool isDirective(const std::string& token)
{
	return (token == "listen" || token == "host" || token == "server_name" || token == "client_max_body_size" || token == "error_page" || token == "root" || token == "index" || token == "location" || token == "allow_methods" || token == "autoindex" || token == "return" || token == "upload_path");
}

void printDirective(const Directive& d)
{
	std::cout << "    Directive: " << d.name << std::endl;
	for (size_t i = 0; i < d.arguments.size(); ++i)
		std::cout << "      arg[" << i << "]: " << d.arguments[i] << std::endl;
}

void printLocation(const LocationNode& loc)
{
	std::cout << "  Location: " << loc.path << std::endl;
	for (size_t i = 0; i < loc.directives.size(); ++i)
		printDirective(loc.directives[i]);
}

void printServer(const ServerNode& server)
{
	std::cout << "Server {" << std::endl;

	// std::cout << "  Directives:" << std::endl;
	for (size_t i = 0; i < server.directives.size(); ++i)
		printDirective(server.directives[i]);

	// std::cout << "  Locations:" << std::endl;
	for (size_t i = 0; i < server.locations.size(); ++i)
		printLocation(server.locations[i]);

	std::cout << "}" << std::endl;
}

std::vector<ServerNode> Parser::parserServerBlock(std::vector<std::string> tokens)
{
	size_t					index = 0;
	std::string 			token;
	ParserState 			state = GLOBAL;
	ServerNode				currentServer;
	LocationNode			currentLocation;
	Directive				currentDirective;
	std::vector<ServerNode>	servers;

	while (index < tokens.size())
	{
		token = tokens[index];
		if (state == GLOBAL)
		{
			if (token == "server" && index + 1 < tokens.size() && tokens[index + 1] == "{")
			{
				currentServer = ServerNode();
				state = SERVER;
				index += 1; // on consomme le token server et { sera consomme à index++
			}
		}
		else if (state == SERVER)
		{
			if (token == "location")
			{
				currentLocation = LocationNode();
				token = tokens[++index];
				currentLocation.path = token;
				state = LOCATION;
			}
			else if (isDirective(token))
			{
				currentDirective = Directive();
				currentDirective.name = token;
				while (token != ";")
				{
					token = tokens[++index];
					if (token != ";")
						currentDirective.arguments.push_back(token);
				}
				currentServer.directives.push_back(currentDirective);
			}
			else if (token == "}")
			{
				// printServer(currentServer);
				servers.push_back(currentServer);
				state = GLOBAL;
			}
		}
		else if (state == LOCATION)
		{
			if (isDirective(token))
			{
				currentDirective = Directive();
				currentDirective.name = token;
				while (token != ";")
				{
					token = tokens[++index];
					if (token != ";")
						currentDirective.arguments.push_back(token);
				}
				currentLocation.directives.push_back(currentDirective);
			}
			else if (token == "}")
			{
				currentServer.locations.push_back(currentLocation);
				state = SERVER;
			}
		}
		index++;
	}
	for (size_t i = 0; i < servers.size(); ++i)
		printServer(servers[i]);
	return (servers);
}

std::vector<ServerNode> Parser::parseFile(const std::string &filename)
{
	std::vector<ServerNode>		servers;
	std::vector<std::string>	tokens;

	tokens = tokenize(filename);
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << tokens[i] << std::endl;
	}
	servers = parserServerBlock(tokens);
	return (servers);
}
