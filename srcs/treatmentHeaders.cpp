#include "../include/response.hpp"
#include "../include/request.hpp"

void    Response::setHeaders(const Request &req)
{
	std::stringstream ss;

	ss << _body.size();

	this->addHeaders("Date", _date);
	this->addHeaders("Server", "werserv/1.1");
	this->addHeaders("Content-Length", ss.str());

	if (_statusCode == METHOD_NOT_ALLOWED)
		this->addHeaders("Allow", req.getSupportedMethod());

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

// const Location* Config::findLocation(std::string uri) const
// {
// 	const Location*	searchLoc = NULL;
// 	size_t			maxLen = 0;

// 	for (size_t i = 0; i < _locations.size(); ++i)
// 	{
// 		if (uri.compare(0, _locations[i].path.length(), _locations[i].path) == 0)
// 		{
// 			if (_locations[i].path.length() >= maxLen)
// 			{
// 				maxLen = _locations[i].path.length();
// 				searchLoc = &_locations[i];
// 			}
// 		}
// 	}
// 	return (searchLoc);
// }