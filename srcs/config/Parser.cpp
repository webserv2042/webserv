#include"../../includes/config/Parser.hpp"

Parser::Parser() {};

std::vector<ServerNode> Parser::parseFile(const std::string& filename)
{
	std::vector<ServerNode>	servers;
	std::vector<std::string> tokens;
	tokens = tokenize(filename);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << std::endl;
	}
	return (servers);
}

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
std::vector<std::string> tokenize(const std::string& filename)
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
