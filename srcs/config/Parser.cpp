#include"../../includes/config/Parser.hpp"

Parser::Parser() {};

bool isDirective(const std::string& token)
{
	return (token == "listen" ||
			token == "host" ||
			token == "server_name" ||
			token == "client_max_body_size" ||
			token == "error_page" ||
			token == "root" ||
			token == "index" ||
			token == "allow_methods" ||
			token == "autoindex" ||
			token == "return" ||
			token == "upload_path");
}

bool isStructuralKeyword(const std::string& token)
{
	return (token == "server" ||
			token == "location");
}

bool isSeparator(char c)
{
	return (c == ' ' || c == '\r' || c == '\n' || c == '{' || c == '}' || c == ';' || c == '\t');
}

bool isSingleCharToken(char c)
{
	return (c == '{' || c == '}' || c == ';');
}

/// @brief affiche la directive et ses arguments
/// @param d directive à afficher
void printDirective(const Directive& d)
{
	std::cout << "    Directive: " << d.name << std::endl;
	for (size_t i = 0; i < d.arguments.size(); ++i)
		std::cout << "      arg[" << i << "]: " << d.arguments[i] << std::endl;
}

/// @brief affiche le bloc Location et ses directives
/// @param loc bloc location à afficher
void printLocation(const LocationNode& loc)
{
	std::cout << "  Location: " << loc.path << std::endl;
	for (size_t i = 0; i < loc.directives.size(); ++i)
		printDirective(loc.directives[i]);
}

/// @brief affiche la configuration du serveur, ses blocs location et directives
/// @param server serveur à afficher
void printServer(const ServerNode& server)
{
	std::cout << "Server {" << std::endl;

	for (size_t i = 0; i < server.directives.size(); ++i)
		printDirective(server.directives[i]);

	for (size_t i = 0; i < server.locations.size(); ++i)
		printLocation(server.locations[i]);

	std::cout << "}" << std::endl;
}

/// @brief Tokenise un fichier de conf en une liste de chaînes
///
/// cette fonction lit un fichier de conf caractère par caractère et le decoupe en tokens
/// exploitables par le parser
///
/// Règles : - Les séparateurs (espace, tab, '\n') delimitent les tokens
///	- {, } et ; sont considere comme des tokens
/// - Les commentaires sont ignorés
///
/// en cas d'échec d'ouverture du fichier, une exception est levée
///
/// @param filename chemin vers le fichier de conf à tokeniser
/// @return std::vector<std::string> liste ordonée des tokens extraits
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

/// @brief Parse les blocs 'server' d'un fichier de configuration tokenisé.
///
/// La fonction effectue un parsing syntaxique strict et transforme la liste de tokems
/// en une structure AST (abstract syntax tree) composée de ServerNode, LocationNode et Directive.
///
/// Règles : - le fichier doit contenir un ou plusieurs bloc 'server{...}'
/// - les blocs 'location{...}' dont autorisés uniquement dans un bloc server
/// - Les delimiteurs { } et ; sont utilisés pour structurer et ne sont pas dans l'AST
///
/// En cas d'erreur syntaxique une exception 'std::runtime_error' est levée avec message
///
/// @param tokens Veteur de tokens produit par tokenisation.
/// @return std::vectir<ServerNode> liste des serveurs parsées depuis la config
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
			if (token == "server")
			{
				index++;
				if (index >= tokens.size() || tokens[index] != "{")
					throw std::runtime_error("(PARSER) expected '{' after 'server'");
				currentServer = ServerNode();
				state = SERVER;
				index++;
			}
			else if (token == "{" || token == "}")
				throw std::runtime_error("(PARSER) unexpected '" + token + "' in GLOBAL context");
			else
				throw std::runtime_error("(PARSER) unexpected token in GLOBAL:" + token);
		}
		else if (state == SERVER)
		{
			if (token == "}")
			{
				servers.push_back(currentServer);
				state = GLOBAL;
				index++;
			}
			else if (token == "location")
			{
				index++;
				if (index >= tokens.size())
					throw std::runtime_error("(PARSER) expected location path after 'location'");
				currentLocation = LocationNode();
				currentLocation.path = tokens[index];
				if (index + 1 < tokens.size())
					index++;
				else
					throw std::runtime_error("(PARSER) unexpected end of file: expected <TOKEN>");
				if (index >= tokens.size() || tokens[index] != "{")
					throw std::runtime_error("(PARSER) expected '{' after location path");
				state = LOCATION;
				index++;
			}
			else if (token == "{")
				throw std::runtime_error("(PARSER) unexpected '{' in SERVER context");
			else // c'est une directive
			{
				currentDirective = Directive();
				currentDirective.name = token;
				if (index + 1 < tokens.size())
					index++;
				else
					throw std::runtime_error("(PARSER) unexpected end of file: expected <TOKEN>");
				while (index < tokens.size() && tokens[index] != ";")
				{
					if (tokens[index] == "{" || tokens[index] == "}")
						throw std::runtime_error("(PARSER) missing ';' after directive '" + currentDirective.name + "'");
					if (isStructuralKeyword(tokens[index]))
						throw std::runtime_error("(PARSER) missing ';' after directive '" +currentDirective.name + "'");
					if (isDirective(tokens[index]))
						throw std::runtime_error("(PARSER) missing ';' after directive '" + currentDirective.name + "'");
					currentDirective.arguments.push_back(tokens[index]);
					index++;
				}
				if (index >= tokens.size())
					throw std::runtime_error("(PARSER) expected ';' after directive'" + currentDirective.name + "'");
				index++;
				currentServer.directives.push_back(currentDirective);
			}
		}
		else if (state == LOCATION)
		{
			if (token == "}")
			{
				currentServer.locations.push_back(currentLocation);
				state = SERVER;
				index++;
			}
			else if (token == "{")
				throw std::runtime_error("(PARSER) unexpected '{' in LOCATION context");
			else
			{
				currentDirective = Directive();
				currentDirective.name = token;
				if (index + 1 < tokens.size())
					index++;
				else
					throw std::runtime_error("(PARSER) unexpected end of file: expected <TOKEN>");
				while (index < tokens.size() && tokens[index] != ";")
				{
					if (tokens[index] == "{" || tokens[index] == "}")
						throw std::runtime_error("(PARSER) missing ';' after directive '" + currentDirective.name + "'");
					if (isStructuralKeyword(tokens[index]))
						throw std::runtime_error("(PARSER) missing ';' after directive '" +currentDirective.name + "'");
					if (isDirective(tokens[index]))
						throw std::runtime_error("(PARSER) missing ';' after directive '" + currentDirective.name + "'");
					currentDirective.arguments.push_back(tokens[index]);
					index++;
				}
				if (index >= tokens.size())
					throw std::runtime_error("(PARSER) expected ';' after directive '" + currentDirective.name + "'");
				index++;
				currentLocation.directives.push_back(currentDirective);
			}
		}
	}
	if (state != GLOBAL)
		 throw std::runtime_error("(PARSER) unexpected end of file: unclosed block");
	for (size_t i = 0; i < servers.size(); ++i)
		printServer(servers[i]);
	return (servers);
}

/// @brief fonction principale qui parse le fichier de configuration
/// @param filename nom du fichier à parser
/// @return liste de serveurs sous forme de vector
std::vector<ServerNode> Parser::parseFile(const std::string &filename)
{
	std::vector<ServerNode>		servers;
	std::vector<std::string>	tokens;

	tokens = tokenize(filename);
	if (tokens.empty())
		throw std::runtime_error("(PARSER) configuration file is empty");
	servers = parserServerBlock(tokens);
	return (servers);
}
