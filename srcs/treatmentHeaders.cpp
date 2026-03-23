#include "../include/response.hpp"
#include "../include/request.hpp"
#include "../include/Config.hpp"

void    Response::setHeaders(const Request &req)
{
	std::stringstream ss;

	ss << _body.size();

	this->addHeaders("Date", _dateHttp);
	this->addHeaders("Server", "werserv/1.1");
	this->addHeaders("Content-Length", ss.str());

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
		_closeFd = false; // a voir si nécéssaire de le garder
	}
}