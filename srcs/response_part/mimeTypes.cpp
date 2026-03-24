#include "../srcs/include/Response.hpp"
#include "../srcs/include/Request.hpp"

std::map<std::string, std::string>	Response::_mimeType;

void	Response::initMimeType()
{
	std::ifstream mimeType("/etc/mime.types");

	if (!mimeType.is_open())
	{
		std::cout << "Erreur ouverture du fichier\n";
		isNotMimeFile();
		return ;
	}

	std::string line;
	while(std::getline(mimeType, line))
	{
		if (line.empty() || line[0] == '#')
			continue ;

		std::stringstream	ss(line);
		std::string			type;
		std::string			extension;

		ss >> type;
		while (ss >> extension)
			_mimeType[extension] = type; //le type deviendra la valeur du header content-type
	}
}

void	Response::isNotMimeFile()
{
	_mimeType["html"] = "text/html";
    _mimeType["htm"] = "text/html";
    _mimeType["css"] = "text/css";
    _mimeType["jpg"] = "image/jpeg";
	_mimeType["jpeg"] = "image/jpeg";	
    _mimeType["png"] = "image/png";
	_mimeType["js"] = "application/javascript";
	_mimeType["json"] = "application/json";
    _mimeType["txt"] = "text/plain"; // par défaut pr le texte
	_mimeType["bin"] = "application/octet-stream"; // par défaut pr tt le reste
}