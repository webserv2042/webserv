#include "../../includes/http/Response.hpp"
#include "../../includes/http/Request.hpp"
#include "../../includes/config/Config.hpp"

void    Response::setHeaders(const Request &req)
{
	std::stringstream ss;

	ss << _body.size();

	this->addHeaders("Date", _dateHttp);
	this->addHeaders("Server", "webserv/1.1");
	this->addHeaders("Content-Length", ss.str());

	if (_headers.find("content-type") == _headers.end())
        this->contentType();

	if (_statusCode == METHOD_NOT_ALLOWED)
		this->addHeaders("Allow", req.getSupportedMethod());

	if (_statusCode >= 300 && _statusCode < 400)
		this->addHeaders("Location", _locationUri);
		
	if (req.getHeader("connection") == "close")
	{
		this->addHeaders("Connection", "close");
		_closeFd = true;
	}
	else
	{
		this->addHeaders("Connection", "keep-alive");
		_closeFd = false;
	}
}

void    Response::contentType()
{
	if (_isAutoIndex)
    {
        this->addHeaders("Content-Type", "text/html");
        return ;
    }

	std::map<std::string, std::string>::iterator it = _mimeType.find(_extension);

	if (it != _mimeType.end())
		this->addHeaders("Content-Type", it->second);
	else
		this->addHeaders("Content-Type", "application/octet-stream");
}
