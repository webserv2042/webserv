#include "../srcs/include/Response.hpp"
#include "../srcs/include/Request.hpp"

void    Response::methodProcess(const Request &req)
{
    std::string method = req.getMethod();

    if (method == "GET")
        this->doGet();
    else if (method == "POST")
        this->doPost(req);
    else if (method == "DELETE")
        this->doDelete();
    else
        this->fail(NOT_IMPLEMENTED);
}

void    Response::doGet()
{
    std::ifstream   file(_uriFullPath.c_str(), std::ios::binary); // on lit le fichier en binaire pour ne rien interpréter

    if (!file.is_open())
    {
        _statusCode = INTERNAL_SERVER_ERROR;
            this->fail(INTERNAL_SERVER_ERROR);    
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
        this->fail(INTERNAL_SERVER_ERROR);

    if (!req.getBody().empty())
        file.write(req.getBody().c_str(), req.getBody().size());
    if (file.fail()) 
    {
        file.close();
        std::remove(_uriFullPath.c_str());
        this->fail(INTERNAL_SERVER_ERROR);
    }

    file.close();
    _statusCode = CREATED;

    std::string msg = "<html><body><h1>Fichier crée avec succès !</h1></body></html>";
    _body.assign(msg.begin(), msg.end()); // assign nettoie puis écrit
}

void    Response::doDelete()
{
    if (S_ISDIR(_dataFile.st_mode)) // je refuse la suppression de dossiers car nginx refuse 
        this->fail(FORBIDDEN);

    if (std::remove(_uriFullPath.c_str()) == 0)
    {
        _statusCode = NO_CONTENT;
        _body.clear();
    }
    else
    {
        if (errno == EACCES || errno == EPERM)
            this->fail(FORBIDDEN);
        else if (errno == ENOENT)
            this->fail(NOT_FOUND);
        else
            this->fail(INTERNAL_SERVER_ERROR);
    }
}