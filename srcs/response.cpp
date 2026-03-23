#include "../include/response.hpp"
#include "../include/request.hpp"
#include "../include/config.hpp"


Response::Response(const Config &configServer) : _statusCode(OK), _date(setHttpDate()), _isCgiExt(false), _config(configServer), _closeFd(false) {}

Response::~Response() {}

void    Response::setResponseFinal(const Request &reqClient)
{
    try 
    {
        this->checkingUri(reqClient);
		if (isCgi())
		{
			CGI 				cgiExec(_uriFullPath, _pathExecCgi);
			std::vector<char>	cgiOutput = cgiExec(reqClient, *this);
			this->parseCgi(cgiOutput);
			this->createResponse();
			return ;
		}
		else
        	this->methodProcess(reqClient);
        this->setHeaders(reqClient);            
    }
    catch (const std::exception &e)
    {
        // this->generateErrorPage(_statusCode);
		this->setHeaders(reqClient);
    }
	this->createResponse();
}

void	Response::createResponse()
{
	this->startLine();

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

//*************************************************************************************************//

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
	_body = bodyHttp;

	std::stringstream ss;
	ss << _body.size();
	addHeaders("content-length", ss.str());
}

void	Response::setHtppDate()
{
	char		buffer[50]; // stocker les données
	time_t		time = time(0); // 
	struct tm	*timeInfo = gmtime(&now);

	stfrtime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

	_date = std::string(buffer);
}


void    Response::setStatusCode(e_status_code code)
{
	_statusCode = code;
}

void    Response::addHeaders(const std::string &key, const std::string &value)
{
	for (size_t i = 0; i < key.length(); ++i)
        key[i] = std::tolower(static_cast<unsigned char>(key[i]));
	_headers[key] = value;
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
	return (_headers[key]);
}

std::string	Response::getExt() const
{
	return (_extension);
}

e_status_code	Response::getStatusCode() const
{
	return (_statusCode);
}
