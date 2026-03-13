#include "client.hpp"

Client::Client() {}

Client::Client(int fd) : _fd(fd), _lastActivity(time(NULL)) {}

Client::~Client() {}

int			Client::getFd() const
{
	return (_fd);
}

Request&    Client::getRequest()
{
	return (_request);
}

void	Client::updateActivity()
{
	_lastActivity = time(NULL);
}

time_t	Client::getLastActivity() const
{
	return (_lastActivity);
}
