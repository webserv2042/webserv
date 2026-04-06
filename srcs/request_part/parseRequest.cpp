#include "../../includes/http/Request.hpp"
#include "../../includes/config/Config.hpp"

void    Request::parseRequest()
{
	while (_step != FINISHED)
	{
		size_t sizeBefore = _bytesData.size();
		if ((_step == START_LINE || _step == HEADERS) && _bytesData.size() > MAX_BYTES)
			this->errorMaxBytes();
		
		switch (_step)
		{
			case START_LINE:
			case HEADERS:
				this->parseLines();
				break ;
			case BODY:
				this->parseBodyContent();
				break ;
			case TRAILERS:
				this->parseTrailers();
				break ;
			case FINISHED:
				return ;
		}
		if (_bytesData.size() == sizeBefore)
			break ; // on att les données
	}
	if (_step == FINISHED)
		_contentLength = _body.size();
}

void    Request::parseLines()
{
	const std::string delim = "\r\n";
	
	while (_step == START_LINE || _step == HEADERS)
	{
		std::vector<char>::iterator it = std::search(_bytesData.begin(), _bytesData.end(), delim.begin(), delim.end());

		if (it == _bytesData.end()) // le delim n'est pas trouvé, on vérif que la taille limite ne soit pas atteinte
		{
			if (_bytesData.size()> MAX_BYTES)
				this->fail((_step == START_LINE) ? URI_TOO_LONG : REQUEST_HEADER_FIELDS_TOO_LARGE);
			return ;
		}

		std::string line(_bytesData.begin(), it);
		_bytesData.erase(_bytesData.begin(), it + 2);

		if (_step == START_LINE)
		{
			if (line.empty()) // recommandé par le RFC d'ignorer les lignes vides potentielles avant le début de la requête
				continue ;
			this->parseStartLine(line);
			_step = HEADERS;
		}
		else if (_step == HEADERS)
		{
			if (line.empty()) // si ligne vide == fin des headers
			{
				if (_allHeaders.count("host") == 0) // si host n'existe pas 
					this->fail(BAD_REQUEST);
				_step = BODY;
				return ;
			}
			
			if (line.find(':') != std::string::npos) // réglage d'un bug, pas de qst ça marche c tout
				this->parseHeaders(line);
			else
				this->fail(BAD_REQUEST);
		}
	}
}

void    Request::parseStartLine(const std::string &line)
{
	size_t  firstSpace = line.find(' ');
	if (firstSpace == std::string::npos)
		this->fail(BAD_REQUEST);
	
	_method = line.substr(0, firstSpace); // extraction de la méthode

	size_t  secondSpace = line.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos || secondSpace == firstSpace + 1)
		this->fail(BAD_REQUEST);
	
	_uri = line.substr((firstSpace + 1), secondSpace - (firstSpace + 1)); // extraction de l'uri
	if (_uri.size() > MAX_BYTES)
    	this->fail(URI_TOO_LONG);
	std::string decodedUri = decodeUri(_uri);
	_uri = parseUri(decodedUri);

	size_t  thirdSpace = line.find(' ', secondSpace + 1);
	if (thirdSpace != std::string::npos)
		this->fail(BAD_REQUEST);
	
	_httpVersion = line.substr(secondSpace + 1); // extraction de la version http

	this->checkStartLine();
}

std::string Request::decodeUri(const std::string &uri)
{
    std::string decoded;

    for (size_t i = 0; i < uri.size(); ++i)
    {
        if (uri[i] == '%' && i + 2 < uri.size()
            && std::isxdigit(uri[i+1]) && std::isxdigit(uri[i+2])) // verif que c'est dans la base hexa 
        {
            int value;
            std::sscanf(uri.c_str() + i + 1, "%2x", &value);
            decoded += static_cast<char>(value);
            i += 2;
        }
		else if (uri[i] == '+') // dans les query string c un espace
			decoded += ' ';
        else
            decoded += uri[i];
    }

    return (decoded);
}

std::string	Request::parseUri(std::string uri)
{

	size_t queryPos = uri.find('?');
    std::string path;

	if (queryPos != std::string::npos)
	{
        _queryString = uri.substr(queryPos + 1); // après le ? 
        path = uri.substr(0, queryPos); // avant le ?
    }
	else
	{
        _queryString = "";
        path = uri;
    }

	size_t pos;
    while ((pos = path.find("//")) != std::string::npos)
    {
		path.erase(pos, 1);
	}
	
	std::vector<std::string>	allPath;
	std::stringstream			ss(path);
	std::string					onePath;

	while(std::getline(ss, onePath, '/'))
	{
		if (onePath == "" || onePath == ".")
			continue ;
		if (onePath == "..")
		{
			if (!allPath.empty())
				allPath.pop_back(); // on supprime le dernier élement
			else
				this->fail(BAD_REQUEST);
		}
		else
			allPath.push_back(onePath);
	}

	std::string cleanPath = "/";
	
	for (size_t i = 0; i < allPath.size(); ++i)
	{
		cleanPath += allPath[i];
		if (i < allPath.size() - 1)
			cleanPath += "/";
	}

	if (path.size() > 1 && path[path.size() - 1] == '/' && cleanPath[cleanPath.size() - 1] != '/')
        cleanPath += "/";
	
	return (cleanPath);
}

void    Request::checkStartLine()
{
	const char* methodNotAllowed[] = {"HEAD", "OPTIONS", "TRACE", "PUT", "PATCH", "CONNECT", NULL};
	bool		found = false;
	// std::string target = "HTTP/1.1";

	for (size_t i = 0; i < _method.size(); ++i)
	{
		if (!std::isupper(_method[i]))
			this->fail(BAD_REQUEST);
	}

	if (_method != "GET" && _method != "POST" && _method != "DELETE")
	{
		for (int i = 0; methodNotAllowed[i] != NULL; i++)
		{
			if (_method == methodNotAllowed[i])
			{
				found = true;
				break ;
			}
		}
		this->fail(found ? METHOD_NOT_ALLOWED : BAD_REQUEST);
	}
	this->parseHttp();
}

void	Request::parseHttp()
{
	if (_httpVersion.size() != 8)
		this->fail(BAD_REQUEST);
	
	if (_httpVersion.substr(0, 5) != "HTTP/")
		this->fail(BAD_REQUEST);
	
	if (!isdigit(_httpVersion[5]))
			this->fail(BAD_REQUEST);
	
	if (_httpVersion[6] != '.')
		this->fail(BAD_REQUEST);
	
	if (!isdigit(_httpVersion[7]))
		this->fail(BAD_REQUEST);

	if (_httpVersion != "HTTP/1.1")
		this->fail(VERSION_NOT_SUPPORTED);
}

void    Request::parseHeaders(const std::string &line)
{
	size_t      delim = line.find(':');

	if (line.size() > 4096)
        this->fail(REQUEST_HEADER_FIELDS_TOO_LARGE);	
    
    if (_allHeaders.size() > 1024)
    {
		this->fail(REQUEST_HEADER_FIELDS_TOO_LARGE);
	}

	if (delim == std::string::npos || (delim > 0 && (line[delim - 1] == ' ' || line[delim - 1] == '\t')))
		this->fail(BAD_REQUEST);

	std::string key = line.substr(0, delim);
	std::string value = line.substr(delim + 1);
	trim(key);
	trim(value);

	key = toLower(key);  // normalisation pr respecter norme rfc (headers sensibles à la casse)

	if (key == "host")
	{
		if (_allHeaders.count("host"))
			this->fail(BAD_REQUEST);
	}
	
	if (key == "cookie")
	{
		this->parseCookie(value);
		return ;
	}
	
	if (key == "content-length")
	{
		char *endPtr;
		long valueKey = std::strtol(value.c_str(), &endPtr, 10);
		if (*endPtr != '\0' || valueKey < 0)
			this->fail(BAD_REQUEST);
		
		if (_allHeaders.count("content-length"))
		{
			if (std::atol(_allHeaders["content-length"].c_str()) != valueKey)
            	this->fail(BAD_REQUEST);
			return ;
		}
		if (valueKey > LIMIT_BODY)
			this->fail(CONTENT_TOO_LARGE);

		if (_allHeaders.count("transfer-encoding") && _allHeaders["transfer-encoding"] == "chunked")
			return ;
	}

	if (key == "transfer-encoding" && value == "chunked")
	{
		_allHeaders.erase("content-length");
	}

	_allHeaders[key] = value;
}

void	Request::parseCookie(const std::string &data)
{
	std::stringstream	ss(data);
	std::string			dataCookie;

	while(std::getline(ss, dataCookie, ';'))
	{
		size_t	equal = dataCookie.find('=');
		if (equal != std::string::npos)
		{
			std::string	key = dataCookie.substr(0, equal);
			std::string value = dataCookie.substr(equal + 1);

			trim(key);
			trim(value);

			_cookies[key] = value;
		}
	}
}

void    Request::parseBodyContent()
{	
	if (_allHeaders.count("transfer-encoding") && _allHeaders["transfer-encoding"] == "chunked")
	{
		this->parseBodyChunked();
		return ;
	}
	if (_allHeaders.count("content-length"))
	{
		if (!_isContentLength)
		{
			long valueForContent = std::atol(_allHeaders["content-length"].c_str()); // on initialise content-length
        
			_contentLength = static_cast<size_t>(valueForContent);
			_isContentLength = true;

			if (_contentLength == 0)
			{
				_step = FINISHED;
				return;
			}
			_body.reserve(_contentLength);
		}

		size_t  sizeNeeded = std::min(_contentLength - _body.size(), _bytesData.size()); // récupérer le min d'octet pr ne pas dépasser
		if (sizeNeeded > 0)
		{
			_body.insert(_body.end(), _bytesData.begin(), _bytesData.begin() + sizeNeeded);
			_bytesData.erase(_bytesData.begin(), _bytesData.begin() + sizeNeeded);
		}

		if (_body.size() == _contentLength)
			_step = FINISHED;
	}
	else // si body vide
	{
		_contentLength = 0;
		_step = FINISHED;
	}
}

void    Request::parseBodyChunked()
{
	while (_step == BODY)
	{
		if (_chunkStep == SEARCH_SIZE)
		{
			std::string delim = "\r\n";
			std::vector<char>::iterator it = std::search(_bytesData.begin(), _bytesData.end(), delim.begin(), delim.end());
			if (it == _bytesData.end())
				return ; // taille incomplète

			std::string hexa(_bytesData.begin(), it);
			char *endPtr;
			if (hexa.empty())
			{
				_bytesData.erase(_bytesData.begin(), it + 2);
				continue ; // si delim est détecté, on relance la boucle instant pour vérifier s'il y a des caractères après
			}
			
			long valueForChunk = std::strtol(hexa.c_str(), &endPtr, 16);
			
			if (*endPtr != '\0' || valueForChunk < 0)
				this->fail(BAD_REQUEST);
			if (static_cast<size_t>(valueForChunk) > LIMIT_BODY ||
			(_body.size() + static_cast<size_t>(valueForChunk)) > LIMIT_BODY)
				this->fail(CONTENT_TOO_LARGE);

			_chunkSize = static_cast<long>(valueForChunk);
			_bytesData.erase(_bytesData.begin(), it + 2);

			if (_chunkSize == 0)
			{
				_step = TRAILERS;
				_contentLength = _body.size();
				return ;
			}
			_chunkStep = READ_DATA;
		}

		if (_chunkStep == READ_DATA)
		{
			if (_bytesData.size() < _chunkSize + 2)
				return ; // pas fini de récupérer les données on att la boucle

			if (_bytesData[_chunkSize] != '\r' || _bytesData[_chunkSize + 1] != '\n')
				this->fail(BAD_REQUEST);

			_body.insert(_body.end(), _bytesData.begin(), _bytesData.begin() + _chunkSize);
			_bytesData.erase(_bytesData.begin(), _bytesData.begin() + (_chunkSize + 2));
			_chunkStep = SEARCH_SIZE;
		}
	}
}

void    Request::parseTrailers() // headers probables après le body
{
	std::string delim = "\r\n";
	while (_step == TRAILERS)
	{
		std::vector<char>::iterator it = std::search(_bytesData.begin(), _bytesData.end(), delim.begin(), delim.end());
		if (it == _bytesData.end())
			return ;
		
		std::string line(_bytesData.begin(), it);
		_bytesData.erase(_bytesData.begin(), it + 2);
	
		if (line.empty())
		{
			_step = FINISHED;
			return ;
		}
		this->parseHeaders(line);
	}
}

void Request::printRequest() const
{
    std::cout << "\033[1;36m--- REQUEST RECEIVED ---\033[0m" << std::endl;
    
    // 1. Start-Line
    std::cout << _method << " " << _uri << " " << _httpVersion << "\r\n";

    // 2. Headers
    std::map<std::string, std::string>::const_iterator it;
    for (it = _allHeaders.begin(); it != _allHeaders.end(); ++it) {
        std::cout << it->first << ": " << it->second << "\r\n";
    }

    // 3. Ligne vide séparatrice
    std::cout << "\r\n";

    // 4. Body (on affiche un aperçu si c'est du texte)
    // if (!_body.empty()) {
    //     if (_body.size() > 500) {
    //         std::string snippet(_body.begin(), _body.begin() + 500);
    //         std::cout << snippet << "\n[... Body truncated, total size: " << _body.size() << " bytes ...]" << std::endl;
    //     } else {
    //         std::string fullBody(_body.begin(), _body.end());
    //         std::cout << fullBody << std::endl;
    //     }

    std::cout << "\033[1;36m----------------------------\033[0m" << std::endl;
}