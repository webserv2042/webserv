#include "../srcs/include/Response.hpp"
#include "../srcs/include/Request.hpp"

void    Response::doAutoIndex(const Request &req)
{
	_isAutoIndex = true;

	DIR				*directory = NULL; // pour ouvrir le dossier
	struct dirent	*fileRead = NULL; // chaque fichier lu
	struct stat		dataFile;
	std::string		uri = req.getUri();

	directory = opendir(_uriFullPath.c_str());
	if (!directory)
		this->fail(FORBIDDEN);
	
	std::stringstream ss;

	ss << "<html><head><title>Index of " + uri + "</title></head>";
	ss << "<body><h1>Index of " + uri + "</h1><hr><ul>";

	while ((fileRead = readdir(directory)) != NULL)
	{
		std::string name = fileRead->d_name;
		if (name == ".")
			continue ;
		
		std::string fullPathWithFile = _uriFullPath; // j'ajoute le fichier à mon path complété plus tôt donc uri + root
		if (fullPathWithFile[fullPathWithFile.size() - 1] != '/')
			fullPathWithFile += '/';
		fullPathWithFile += name;

		std::string	isDirectory = name;
		if (stat(fullPathWithFile.c_str(), &dataFile) == 0)
		{
			if (S_ISDIR(dataFile.st_mode))
				isDirectory += "/";
		}
		ss << "<li><a href=\"" + isDirectory + "\">" + isDirectory + "</a></li>";
	}

	closedir(directory);
	ss << "</ul><hr></body></html>";

	std::string html = ss.str();
	_body.assign(html.begin(), html.end());
	_statusCode = OK;
}