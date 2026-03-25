#include "../../includes/http/Request.hpp"

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
	_isContentLength(false),
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

bool		Request::isFinished() const
{
	return (_step == FINISHED);
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

std::vector<char>	Request::getRequest() const
{
	return (_bytesData);
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
