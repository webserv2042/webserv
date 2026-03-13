#include "../include/response.hpp"
#include "../include/request.hpp"


Response::Response() : _statusCode(OK), _isCgiExt(false) {}

Response::~Response() {}

// void    Response::setResponseFinal(const Request &reqClient)
// {
// 	std::stringstream ss;
// 	ss << "HTTP/1.1 " << _statusCode << " " << _statusMessage[_statusCode] << "\r\n";

// 	std::string	startLine = ss.str();
// 	_responseFinal.insert(_responseFinal.end(), startLine.begin(), startLine.end());

// }

//Fonctions pour setup les différentes partie de la réponse + ajouter le code status    
void    Response::setBody(const std::string &bodyHttp)
{
	_body = bodyHttp;

	std::stringstream ss;
	ss << _body.size();
	addHeaders("content-length", ss.str());
}

void    Response::setStatusCode(e_status_code code)
{
	_statusCode = code;
}

void    Response::addHeaders(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

// Dans Response.hpp
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