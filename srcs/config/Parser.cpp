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

void	Parser::validateArgumentCount(const Directive& d)
{
	size_t count = d.arguments.size();

	if (d.name == "listen" && count != 1)
		throw std::runtime_error("(CONFIG) 'listen' exptects exactly 1 argument");
	if (d.name == "host" && count != 1)
		throw std::runtime_error("(CONFIG) 'host' exptects exactly 1 argument");
	if (d.name == "root" && count != 1)
		throw std::runtime_error("(CONFIG) 'root' exptects exactly 1 argument");
	if (d.name == "server_name" && count < 1)
		throw std::runtime_error("(CONFIG) 'server_name' exptects at least 1 argument");
	if (d.name == "index" && count < 1)
		throw std::runtime_error("(CONFIG) 'index' exptects at least 1 argument");
	if (d.name == "error_page" && count != 2)
		throw std::runtime_error("(CONFIG) 'error_page' expects exactly 2 argument");
	if (d.name == "client_max_body_size" && count != 1)
		throw std::runtime_error("(CONFIG) 'client_max_body_size' exptects exactly 1 argument");
	if (d.name == "allow_methods" && count < 1)
		throw std::runtime_error("(CONFIG) 'allow_methods' exptects at least 1 argument");
	if (d.name == "autoindex" && count != 1)
		throw std::runtime_error("(CONFIG) 'autoindex' exptects exactly 1 argument");
	if (d.name == "return" && count != 2)
		throw std::runtime_error("(CONFIG) 'return' exptects exactly 2 argument");
	if (d.name == "upload_path" && count != 1)
		throw std::runtime_error("(CONFIG) 'upload_path' exptects exactly 1 argument");
}


bool	Parser::isDirectiveAllowedInContext(const std::string& name, Context ctx)
{
	if (ctx == SERVER_CTX)
	{
		return (name == "listen" || name == "host" || name == "server_name" || name == "root" ||
				name == "index" || name == "error_page" || name == "client_max_body_size");
	}
	else
	{
		return (name == "root" || name == "allow_methods" || name == "autoindex" || name == "return" ||
				name == "index" || name == "upload_path");
	}
}


void	Parser::validateDirective(const Directive& d, Context ctx)
{
	if (!isDirectiveAllowedInContext(d.name, ctx))
	{
		std::string ctxName;
		if (ctx == SERVER_CTX)
			ctxName = "server";
		else
			ctxName = "location";
		throw std::runtime_error("(CONFIG) directive '" + d.name +
								"' is not allowed in " + ctxName + " context");
	}
	validateArgumentCount(d);
	validateArgumentValues(d);
}


void	Parser::validateLocation(const LocationNode& location)
{
	if (location.path.empty())
		throw std::runtime_error("(CONFIG) location path cannot be empty");
	for (size_t i = 0; i < location.directives.size(); ++i) {
		validateDirective(location.directives[i], LOCATION_CTX);
	}
}


void	Parser::validateServer(const ServerNode& server)
{
	bool	hasListen = false;
	size_t	hostCount = 0;
	size_t	rootCount = 0;
	size_t	clientSizeCount = 0;
	size_t	indexCount = 0;

	for (size_t i = 0; i < server.directives.size(); ++i) {
		const Directive& d = server.directives[i];
		if (d.name == "listen")
			hasListen = true;
		if (d.name == "host")
			hostCount++;
		if (d.name == "root")
			rootCount++;
		if (d.name == "client_max_body_size")
			clientSizeCount++;
		if (d.name == "index")
			indexCount++;
		validateDirective(d, SERVER_CTX);
	}
	if (!hasListen)
		throw std::runtime_error("(CONFIG) missing 'listen' directive in server block");
	if (hostCount > 1)
		throw std::runtime_error("(CONFIG) 'host' can appear at most once in server block");
	if (rootCount > 1)
		throw std::runtime_error("(CONFIG) 'root' can appear at most once in server block");
	if (clientSizeCount > 1)
		throw std::runtime_error("(CONFIG) 'client_max_body_size' can appear at most once in server block");
	if (indexCount > 1)
		throw std::runtime_error("(CONFIG) 'index' can appear at most once in server block");
	for (size_t i = 0; i < server.locations.size(); ++i) {
		validateLocation(server.locations[i]);
	}
}


void	Parser::validateServers(const std::vector<ServerNode>& servers)
{
	if (servers.empty())
		throw std::runtime_error("(CONFIG) no server block defined");

	for (size_t i = 0; i < servers.size(); ++i) {
		validateServer(servers[i]);
	}
}

/// @brief fonction principale qui parse le fichier de configuration
/// @param filename nom du fichier à parser
/// @return liste de serveurs sous forme de vector
std::vector<ServerNode> Parser::parseFile(const std::string& filename)
{
	std::vector<ServerNode>		servers;
	std::vector<std::string>	tokens;

	tokens = tokenize(filename);
	if (tokens.empty())
		throw std::runtime_error("(PARSER) configuration file is empty");
	servers = parserServerBlock(tokens);
	validateServers(servers);
	return (servers);
}
