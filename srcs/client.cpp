#include "../include/Client.hpp"

Client::Client(int fd, const Config *config) :
						clientFd(fd),
						serverConfig(config)
					{
						this->_lastActivity = time(NULL);
					}

Client::~Client() {}

int			Client::getFd() const
{
	return (clientFd);
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
