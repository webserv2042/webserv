#include "../include/response.hpp"
#include "../include/request.hpp"

void    Response::checkingUri(const Request &req)
{
    this->fullPathUri(req);

    if (stat(_uriFullPath.c_str(), &_dataFile) != 0) // uri introuvable
    {
        _statusCode = NOT_FOUND;
        throw std::exception(); // a modifier pour generer page d'erreur
        return ;
    }
    
    if (S_ISREG(_dataFile.st_mode)) // un fichier ?
        this->checkingPerm();
    else if(S_ISDIR(_dataFile.st_mode)) // un dossier ?
        this->searchFile();

    this->contentType();
}

void    Response::checkingPerm()
{
    if (!(_dataFile.st_mode & S_IRUSR)) // autorisation de l'user de lire ?
    {
        _statusCode = FORBIDDEN;
        throw std::exception();
        return ;
    }
    
    _statusCode = OK;
}

void    Response::searchFile()
{
    std::string fileOnDirectory = _uriFullPath;

    if (fileOnDirectory[fileOnDirectory.size() - 1] != '/')
        fileOnDirectory += "/";
    fileOnDirectory += "index.html";

    if (stat(fileOnDirectory.c_str(), &_dataFile) != 0) // fichier introuvable
    {
        _statusCode = NOT_FOUND;
        throw std::exception();
        return ;
    }
    _uriFullPath = fileOnDirectory;
    this->checkingPerm();
    this->setExtension();
}

void    Response::fullPathUri(const Request &req)
{
    std::string root = "www";
    std::string uri = req.getUri();
    // std::string uri = "/index.html";

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
    if (_extension == "php" || _extension == "py" || _extension == "sh")
        return (true);
    return (false);
}

void    Response::contentType()
{
    if (this->isCgi())
        return ; // ->j'envoie à la cgi
    
    if (_mimeType.count(_extension))
        _headers["content-type"] = _mimeType[_extension];
    else
        _headers["content-type"] = "application/octet-stream";
}