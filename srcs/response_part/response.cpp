#include "../../include/Response.hpp"
#include "../../include/Request.hpp"
#include "../../include/Config.hpp"
#include "../../include/Cgi.hpp"

Response::Response(const Config &configServer) :
	_statusCode(OK),
    _isCgiExt(false),
    _config(configServer),
	_structLocation(NULL),
    _isAutoIndex(false),
	_closeFd(false)
{
	_responseFinal.clear();
    _body.clear();
    _headers.clear();

	this->setHttpDate();
}

Response::~Response() {}

void    Response::setResponseFinal(const Request &reqClient)
{
	if (_statusCode >= 300 && _statusCode < 400)
	{
        this->setHeaders(reqClient);
        this->createResponse();
        return ;
    }

    try 
    {
        this->checkingUri(reqClient);
		if (isCgi())
		{
			CGI 				cgiExec(_uriFullPath, _pathExecCgi);
			std::vector<char>	cgiOutput = cgiExec.cgi(reqClient, *this);
			this->parseCgi(cgiOutput);
			// this->setHeaders(reqClient);
			this->createResponse();
			return ;
		}
		else
        	this->methodProcess(reqClient);
        this->setHeaders(reqClient);            
    }

    catch (const std::exception &e)
    {
        this->generateErrorPage(_statusCode);
		this->setHeaders(reqClient);
    }
	this->createResponse();
}

void	Response::createResponse()
{
	_responseFinal.clear();
	this->setStartLine();

	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::string line = it->first + ": " + it->second + "\r\n";
		_responseFinal.insert(_responseFinal.end(), line.begin(), line.end());
	}

	std::string	lineEmpty = "\r\n";
	_responseFinal.insert(_responseFinal.end(), lineEmpty.begin(), lineEmpty.end());

	if (!_body.empty())
		_responseFinal.insert(_responseFinal.end(), _body.begin(), _body.end());
}

void	Response::setStartLine()
{
	std::stringstream ss;
	ss << "HTTP/1.1 " << _statusCode << " " << _statusMessage[_statusCode] << "\r\n";

	std::string	startLine = ss.str();
	_responseFinal.insert(_responseFinal.end(), startLine.begin(), startLine.end());
}

//Fonctions pour setup les différentes partie de la réponse + ajouter le code status    
void    Response::setBodySize(const std::string &bodyHttp)
{
	_body.assign(bodyHttp.begin(), bodyHttp.end());

	std::stringstream ss;
	ss << _body.size();
	addHeaders("content-length", ss.str());
}

void	Response::setHttpDate()
{
	char		buffer[50]; // stocker les données
	time_t		now = time(0); // 
	struct tm	*timeInfo = gmtime(&now);

	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

	_dateHttp = std::string(buffer);
}

void	Response::setLocationUri(const std::string &path)
{
	_locationUri = path;
}

void    Response::setStatusCode(e_status_code code)
{
	_statusCode = code;
}

void    Response::addHeaders(const std::string &key, const std::string &value)
{
	std::string lowerKey = key;

	lowerKey = toLower(lowerKey);

	_headers[lowerKey] = value;
}

std::string Response::getUriFullPath() const
{
	return (_uriFullPath);
}

std::string Response::getExtension() const
{
	return (_extension);
}

std::string Response::getHeader(std::string key)
{
    std::map<std::string, std::string>::iterator it = _headers.find(toLower(key));

    if (it != _headers.end())
        return it->second;
    return "";
}

std::string	Response::getExt() const
{
	return (_extension);
}

e_status_code	Response::getStatusCode() const
{
	return (_statusCode);
}

bool		Response::getCloseFd() const
{
	return (_closeFd);
}

const std::vector<char> & Response::getResponseFinal() const
{
	return (_responseFinal);
}
