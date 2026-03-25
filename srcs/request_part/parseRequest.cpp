#include "../../includes/http/Request.hpp"

void    Request::parseRequest()
{
	while (_step != FINISHED)
	{
		size_t sizeBefore = _bytesData.size();
		if ((_step == START_LINE || _step == HEADERS) && _bytesData.size() > MAX_BYTES)
		{
			this->errorMaxBytes();
			return ;
		}
		
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
			this->errorMaxBytes();
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
				std::cout << "DEBUG: Ligne ignorée car pas de ':' -> [" << line << "]" << std::endl;
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

	size_t queryPos = _uri.find('?');
	if (queryPos != std::string::npos)
	{
		_queryString = _uri.substr(queryPos + 1); // Ce qui est après le ?
		_uri = _uri.substr(0, queryPos);          // Ce qui est avant le ?
	}
	else
		_queryString = ""; // Important de vider si pas de ?

	size_t  thirdSpace = line.find(' ', secondSpace + 1);
	if (thirdSpace != std::string::npos)
		this->fail(BAD_REQUEST);
	
	_httpVersion = line.substr(secondSpace + 1); // extraction de la version http

	this->checkStartLine();
}

void    Request::checkStartLine()
{
	const char* methodNotAllowed[] = {"HEAD", "OPTIONS", "TRACE", "PUT", "PATCH", "CONNECT", NULL};
	bool		found = false;
	std::string target = "HTTP/1.1";

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
		if (found == true)
			_errorCode = METHOD_NOT_ALLOWED;
		else
			_errorCode = BAD_REQUEST;
	}

	if (_httpVersion != target)
		this->fail(VERSION_NOT_SUPPORTED);
}

void    Request::parseHeaders(const std::string &line)
{
	size_t      delim = line.find(':');

	if (delim == std::string::npos || (delim > 0 && (line[delim - 1] == ' ' || line[delim - 1] == '\t')))
		this->fail(BAD_REQUEST);

	std::string key = line.substr(0, delim);
	std::string value = line.substr(delim + 1);
	trim(key);
	trim(value);

	key = toLower(key);  // normalisation pr respecter norme rfc (headers sensibles à la casse)
	
	// if (key == "cookie")
	// 	this->parseCookie(value);
	
	if (key == "content-length")
	{
		char *endPtr;
		long valueKey = std::strtol(value.c_str(), &endPtr, 10);
		if (*endPtr != '\0' || valueKey < 0)
		{
			_errorCode = BAD_REQUEST;
			throw std::exception();
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

// void	Request::parseCookie(const std::string &value)
// {
// 	std::string target = "id=";
// 	size_t		
// }

void    Request::parseBodyContent()
{
	if (_allHeaders.count("transfer-encoding") && _allHeaders["transfer-encoding"] == "chunked")
	{
		this->parseBodyChunked();
		return ;
	}
	if (_allHeaders.count("content-length"))
	{
		if (_contentLength == 0)
		{
			long valueForContent = std::atol(_allHeaders["content-length"].c_str()); // on initialise content-length
			_contentLength = static_cast<size_t>(valueForContent);

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
			if (*endPtr != '\0' || valueForChunk < 0 || (_body.size() + static_cast<size_t>(valueForChunk)) > LIMIT_BODY)
				this->fail(CONTENT_TOO_LARGE);

			_chunkSize = static_cast<size_t>(valueForChunk);
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
			{
				std::cout << "DEBUG: Attente data. Size actuelle: " << _bytesData.size() << " Besoin: " << _chunkSize + 2 << std::endl;
				return ; // pas fini de récupérer les données on att la boucle
			}

			if (_bytesData[_chunkSize] != '\r' || _bytesData[_chunkSize + 1] != '\n')
				this->fail(BAD_REQUEST);

			std::cout << "DEBUG: Insertion de " << _chunkSize << " octets." << std::endl;
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
