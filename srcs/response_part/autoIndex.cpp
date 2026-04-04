#include "../../includes/http/Response.hpp"
#include "../../includes/http/Request.hpp"

// void    Response::doAutoIndex(const Request &req)
// {
// 	_isAutoIndex = true;

// 	DIR				*directory = NULL; // pour ouvrir le dossier
// 	struct dirent	*fileRead = NULL; // chaque fichier lu
// 	struct stat		dataFile;
// 	std::string		uri = req.getUri();

// 	directory = opendir(_uriFullPath.c_str());
// 	if (!directory)
// 		this->fail(FORBIDDEN);
	
// 	std::stringstream ss;

// 	ss << "<html><head><title>Index of " + uri + "</title></head>";
// 	ss << "<body><h1>Index of " + uri + "</h1><hr><ul>";

// 	while ((fileRead = readdir(directory)) != NULL)
// 	{
// 		std::string name = fileRead->d_name;
// 		if (name == ".")
// 			continue ;
		
// 		std::string fullPathWithFile = _uriFullPath; // j'ajoute le fichier à mon path complété plus tôt donc uri + root
// 		if (fullPathWithFile[fullPathWithFile.size() - 1] != '/')
// 			fullPathWithFile += '/';
// 		fullPathWithFile += name;

// 		std::string	isDirectory = name;
// 		if (stat(fullPathWithFile.c_str(), &dataFile) == 0)
// 		{
// 			if (S_ISDIR(dataFile.st_mode))
// 				isDirectory += "/";
// 		}
// 		ss << "<li><a href=\"" + isDirectory + "\">" + isDirectory + "</a></li>";
// 	}
// 	// std::cerr << "html size: " << html.size() << std::endl;
// 	closedir(directory);
// 	ss << "</ul><hr></body></html>";

// 	std::string html = ss.str();
// 	_body.assign(html.begin(), html.end());
// 	// std::cerr << "body size apres assign: " << _body.size() << std::endl;
// 	_statusCode = OK;
// }

void    Response::doAutoIndex(const Request &req)
{
    _isAutoIndex = true;
    DIR             *directory = NULL;
    struct dirent   *fileRead = NULL;
    struct stat     dataFile;
    std::string     uri = req.getUri();

    directory = opendir(_uriFullPath.c_str());
    if (!directory)
        this->fail(FORBIDDEN);
    
    std::stringstream ss;

    // --- STYLE CSS INTEGRE ---
    ss << "<html><head><title>Index of " << uri << "</title>";
    ss << "<style>"
       << "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 40px; background-color: #f4f7f6; color: #333; }"
       << "h1 { color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 10px; }"
       << "table { width: 100%; border-collapse: collapse; background: white; box-shadow: 0 4px 6px rgba(0,0,0,0.1); border-radius: 8px; overflow: hidden; }"
       << "th, td { text-align: left; padding: 12px 20px; border-bottom: 1px solid #eee; }"
       << "th { background-color: #3498db; color: white; text-transform: uppercase; font-size: 0.85em; letter-spacing: 1px; }"
       << "tr:hover { background-color: #f9f9f9; }"
       << "a { color: #3498db; text-decoration: none; font-weight: 600; }"
       << "a:hover { color: #2980b9; text-decoration: underline; }"
       << ".size { color: #7f8c8d; font-family: monospace; }"
       << "</style></head>";

    ss << "<body><h1>Index of " << uri << "</h1>";
    ss << "<table><thead><tr><th>Name</th><th>Last modification</th><th>Size</th></tr></thead><tbody>";

    while ((fileRead = readdir(directory)) != NULL)
    {
        std::string name = fileRead->d_name;
        
        // On ignore le dossier actuel (.)
        if (name == ".")
            continue ;
        
        // Construction du path complet pour le stat()
        std::string fullPathWithFile = _uriFullPath;
        if (fullPathWithFile[fullPathWithFile.size() - 1] != '/')
            fullPathWithFile += '/';
        fullPathWithFile += name;

        std::string displayName = name;
        std::string sizeStr = "-";
        std::string dateStr = "-";

        if (stat(fullPathWithFile.c_str(), &dataFile) == 0)
        {
            // Formatage de la date (ex: 29-Mar-2026 20:55)
            char dateBuf[20];
            strftime(dateBuf, 20, "%d-%b-%Y %H:%M", localtime(&dataFile.st_mtime));
            dateStr = dateBuf;

            if (S_ISDIR(dataFile.st_mode))
                displayName += "/";
            else
                sizeStr = toString(dataFile.st_size) + " octets";
        }

        ss << "<tr>"
           << "<td><a href=\"" << displayName << "\">" << displayName << "</a></td>"
           << "<td>" << dateStr << "</td>"
           << "<td class='size'>" << sizeStr << "</td>"
           << "</tr>";
    }

    closedir(directory);
    ss << "</tbody></table><footer style='margin-top:20px; font-size:0.8em; color: #bdc3c7;'>webserv/1.1</footer></body></html>";

    std::string html = ss.str();
    _body.assign(html.begin(), html.end());

    // --- MISE À JOUR CRUCIALE DU CONTENT-LENGTH ---
    // On efface les doublons potentiels et on met la taille exacte du HTML généré

    _statusCode = OK;
}