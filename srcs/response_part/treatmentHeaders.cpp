#include "../../includes/http/Response.hpp"
#include "../../includes/http/Request.hpp"
#include "../../includes/config/Config.hpp"

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
		_closeFd = false;
	}
}

// void    Response::setHeaders(const Request &req)
// {
//     std::stringstream ss;
//     ss << _body.size();

//     this->addHeaders("Date", _dateHttp);
//     this->addHeaders("Server", "werserv/1.1");
//     this->addHeaders("Content-Length", ss.str());

//     if (_statusCode == METHOD_NOT_ALLOWED)
//         this->addHeaders("Allow", req.getSupportedMethod());

//     if (_statusCode >= 300 && _statusCode < 400)
//         this->addHeaders("Location", _locationUri);
        
//     // UTILISE UNE VARIABLE LOCALE POUR ÉVITER LE CRASH
//     std::string conn;
//     try {
//         conn = req.getHeader("connection");
//     } catch (...) {
//         conn = "keep-alive"; // Valeur par défaut si le header n'existe pas
//     }

//     if (conn == "close")
//     {
//         this->addHeaders("Connection", "close");
//         _closeFd = true;
//     }
//     else
//     {
//         this->addHeaders("Connection", "keep-alive");
//         _closeFd = false;
//     }
// }