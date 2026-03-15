#include "../include/response.hpp"
#include "../include/request.hpp"

void    Response::methodProcess(const Request &req)
{
    std::string method = req.getMethod();

    try
    {
        if (method == "GET")
            this->doGet();
        else if (method == "POST")
            this->doPost(req);
        else if (method == "DELETE")
            this->doDelete();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n'; // ->generer page d'erreur
    }

}

void    Response::doGet()
{
    std::ifstream   file(_uriFullPath.c_str(), std::ios::binary); // on lit le fichier en binaire pour ne rien interpréter

    if (!file.is_open())
    {
        _statusCode = INTERNAL_SERVER_ERROR;
        return ;
    }

    _body.resize(_dataFile.st_size); // on alloue la mémoire + on remplit le body

    if (_dataFile.st_size > 0)
        file.read(&_body[0], _dataFile.st_size);

    file.close();
    _statusCode = OK;
}

void    Response::doPost(const Request &req)
{
    struct stat checkFile;
    
    if (stat(_uriFullPath.c_str(), &checkFile) == 0)
    {
        _statusCode = CONFLICT;
        std::string msg = "<html><body><h1>Conflit : Le fichier existe déjà.</h1></body></html>";
        _body.assign(msg.begin(), msg.end());
        return ;
    }

    std::ofstream  file(_uriFullPath.c_str(), std::ios::binary);

    if (!file.is_open())
    {
        _statusCode = INTERNAL_SERVER_ERROR;
        return ;
    }

    if (!req.getBody().empty())
        file.write(req.getBody().c_str(), req.getContentLength());

    file.close();
    _statusCode = CREATED;

    std::string msg = "<html><body><h1>Fichier crée avec succès !</h1></body></html>";
    _body.assign(msg.begin(), msg.end()); // assign nettoie puis écrit
}

void    Response::doDelete()
{
    if (S_ISDIR(_dataFile.st_mode)) // je refuse la suppression de dossiers car nginx refuse 
    {
        _statusCode = FORBIDDEN;
        throw std::exception();
    }

    if (std::remove(_uriFullPath.c_str()) == 0)
    {
        _statusCode = NO_CONTENT;
        _body.clear(); // ATTENTION EN CAS DE DOUBLE FREE VOIR ICI !!!!
    }
    else
    {
        if (errno == EACCES || errno == EPERM)
            _statusCode = FORBIDDEN;
        else if (errno == ENOENT)
            _statusCode = NOT_FOUND;
        else
            _statusCode = INTERNAL_SERVER_ERROR;
        throw std::exception();
    }
}