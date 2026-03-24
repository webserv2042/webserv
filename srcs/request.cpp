#include "../include/request.hpp"

Request::Request() :
	_bytesData(),
	_allHeaders(),
	_method(""),
	_uri(""),
	_queryString(""),
	_httpVersion(""),
	_body(""),
	_cookies(""),
	_contentLength(0),
	_chunkSize(0),
	_methodEnum(DEFAULT),
	_step(START_LINE),
	_chunkStep(SEARCH_SIZE),
	_errorCode(OK)
{
	_bytesData.clear();
    _allHeaders.clear();
}

Request::~Request() {}

void Request::reset()
{
    _bytesData.clear();
    _allHeaders.clear();
    _method = "";
    _uri = "";
    _queryString = "";
    _httpVersion = "";
    _body = "";
    _cookies = "";
    _contentLength = 0;
    _chunkSize = 0;
    _methodEnum = DEFAULT;
    _step = START_LINE;
    _chunkStep = SEARCH_SIZE;
    _errorCode = OK;
}

void		Request::feeding(const char *buffer, size_t sizeOfBytes)
{
	_bytesData.insert(_bytesData.end(), buffer, buffer + sizeOfBytes);

	if (_step != FINISHED)
		{
			try {
				this->parseRequest();
			} catch (const std::exception &e) {
				_step = FINISHED; // On force la fin si erreur détectée
			}
		}
}

bool		Request::isFinished() const
{
	return (_step == FINISHED);
}

void	Request::setMethod(const std::string &method)
{
	if (method == "GET")
		_methodEnum = GET;
	else if (method == "POST")
		_methodEnum = POST;
	else if (method == "DELETE")
		_methodEnum = DELETE;
	else
		_methodEnum = NOT_ALLOWED; // renvoyer une erreur directement ? _errorCode ? 
}

std::string Request::getMethod() const
{
	return (_method);
}

std::string Request::getUri() const
{
	return (_uri);
}

std::string	Request::getHttpVersion() const
{
	return (_httpVersion);
}

std::string	Request::getBody() const
{
	return (_body);
}

size_t	Request::getContentLength() const
{
	return (_contentLength);
}

const std::map<std::string, std::string> &Request::getAllHeaders() const
{
	return (_allHeaders);
}

std::string Request::getHeader(const std::string& key) const
{
    std::string lowerKey = key;

    lowerKey = toLower(lowerKey);

    std::map<std::string, std::string>::const_iterator it = _allHeaders.find(lowerKey);

    if (it != _allHeaders.end())
        return (it->second) ;

    return ("") ;
}

std::string	Request::getSupportedMethod() const
{
	return ("GET, POST, DELETE");
}

e_parsing_steps	Request::getStep() const
{
	return (_step);
}

e_status_code	Request::getErrorCode() const
{
	return (_errorCode);
}

std::string	Request::getQueryString() const
{
	return (_queryString);
}

//============================================================================================================================//
//(a mettre dans fonction liée à la class webserv)
// Fonction qui lit les octets reçus de la boucle epoll et vérifie ce qu'elle va envoyer a la partie réception de la requête
// premier filtrage !

// int		Webserv::readRequest(int fd)
// {
// 	char    buffer[4096];
// 	ssize_t bytesReceived;

// 	bytesReceived = recv(fd, buffer, sizeof(buffer), 0);

// 	if (bytesReceived > 0)
// 	{
// 		_client[fd].updateActivity();
// 		_client[fd].getRequest().feeding(buffer, (size_t)bytesReceived); // on récup le client du fd nommé, on copie les octets reçus du buffer vers sa requête
// 		if (_client[fd].getRequest().isFinished())
// 		{
// 			std::cout << "Requête terminée du fd " << fd << " !" << std::endl;
// 		}
// 	}
// 	else if (bytesReceived == 0)
// 	{
// 		std::cout << "Client fermé avec succès !" << std::endl;
// 		return DISCONNECTION; // -> implémentation à revoir
// 	}
// 	else
// 	{
// 		std::cout << "Erreur, client fermé." << std::endl;
// 		return DISCONNECTION;
// 	}
// 	// regrouper les deux if ? à voir selon cas d'erreur et fonction de fermeture, si une seule ou deux distinctes (closeError et closeSuccess)
// }
