#include "../include/response.hpp"
#include "../include/request.hpp"
#include "../include/Config.hpp"

void    Response::checkingUri(const Request &req)
{
	_structLocation = _config.findLocation(req.getUri());

	if (_structLocation && _structLocation->returnRedirect.first != 0)
	{
		_statusCode = static_cast<e_status_code>(_structLocation->returnRedirect.first);
		_locationUri = _structLocation->returnRedirect.second;
		this->fail(_statusCode);
	}

	this->fullPathUri(req);

	if (stat(_uriFullPath.c_str(), &_dataFile) != 0) // uri introuvable
		this->fail(NOT_FOUND);
	
	if (S_ISREG(_dataFile.st_mode)) // un fichier ?
		this->checkingPerm();
	else if(S_ISDIR(_dataFile.st_mode)) // un dossier ?
		this->searchFile(req);

	this->contentType();
}

const Location* Config::findLocation(std::string uri) const
{
	const Location*	searchLoc = NULL;
	size_t			maxLen = 0;

	for (size_t i = 0; i < _locations.size(); ++i)
	{
		if (uri.compare(0, _locations[i].path.length(), _locations[i].path) == 0)
		{
			if (_locations[i].path.length() >= maxLen)
			{
				maxLen = _locations[i].path.length();
				searchLoc = &_locations[i];
			}
		}
	}
	return (searchLoc);
}

void    Response::checkingPerm()
{
	if (!(_dataFile.st_mode & S_IRUSR)) // autorisation de l'user de lire ?
		this->fail(FORBIDDEN);
	
	_statusCode = OK;
}

void    Response::searchFile(const Request &req)
{
	std::string fileOnDirectory = _uriFullPath;

	if (fileOnDirectory[fileOnDirectory.size() - 1] != '/')
		fileOnDirectory += "/";
	fileOnDirectory += "index.html";

	if (stat(fileOnDirectory.c_str(), &_dataFile) == 0)
	{
		_uriFullPath = fileOnDirectory;
		this->checkingPerm();
		this->setExtension();
	}
	else if (_structLocation && _structLocation->autoIndex == true)
		this->doAutoIndex(req);
	else
		this->fail(NOT_FOUND);
}

void    Response::fullPathUri(const Request &req)
{
	std::string root;
	std::string uri = req.getUri();
	// std::string uri = "/index.html";

	if (_structLocation && !_structLocation->root.empty())
		root = _structLocation->root;
	else if (!_config.getRoot().empty())
		root = _config.getRoot();
	else
		root = "www";

	if (!root.empty() && root[root.size() - 1] == '/' && !uri.empty() && uri[0] == '/')
		_uriFullPath = root + uri.substr(1);
	else
		_uriFullPath = root + uri;

	this->setExtension();
}

void    Response::setExtension()
{
	size_t ext = _uriFullPath.find_last_of('.'); // on cherche la pos du dernier '.'

	if (ext == std::string::npos || ext == _uriFullPath.size() - 1)
	{
		_extension = "";
		return ;
	}

	_extension = _uriFullPath.substr(ext + 1);
}

bool    Response::isCgi()
{
	if (!_structLocation)
		return (false);
	
	std::map<std::string, std::string>::const_iterator it = _structLocation->cgi.find("." + _extension);

	if (it != _structLocation->cgi.end())
	{
		_pathExecCgi = it->second;
		return (true);
	}

	return (false);
}

void    Response::contentType()
{
	if (this->isCgi())
		return ; // ->j'envoie à la cgi qui renverra elle-mm son content-type

	if (_isAutoIndex)
    {
        this->addHeaders("Content-Type", "text/html");
        return ;
    }

	std::map<std::string, std::string>::iterator it = _mimeType.find(_extension);

	if (it != _mimeType.end())
		this->addHeaders("Content-Type", it->second);
	else
		this->addHeaders("Content-Type", "application/octet-stream");
}