#include "../../includes/server/Client.hpp"

Client::Client() : 
	clientFd(-1), 
	clientState(CONNEXION_ACCEPTED), 
	bytesSent(0), 
	buffSize(0), 
	_lastActivity(time(NULL)), 
	_serverConfig(NULL), 
	_keepAlive(true) 
{}

Client::Client(int fd, const Config *config, int ep_fd) : 
	clientFd(fd), 
	clientState(CONNEXION_ACCEPTED),
	epFd(ep_fd),
	bytesSent(0), 
	buffSize(0), 
	_lastActivity(time(NULL)), 
	_serverConfig(config), 
	_keepAlive(true),
	isCGI(0),
	pipeType(0),
	ogFd(-1),
	forkPid(-1)
{}

Client::Client(const Client& to_copy)
{
	*this = to_copy;
}

Client& Client::operator=(const Client& to_copy)
{
	if (this != &to_copy)
	{
		this->clientFd = to_copy.clientFd;
		this->clientState = to_copy.clientState;
		this->epFd = to_copy.epFd;

		this->writeBuff = to_copy.writeBuff;
		this->bytesSent = to_copy.bytesSent;
		this->buffSize = to_copy.buffSize;
		this->_lastActivity = to_copy._lastActivity;

		this->_serverConfig = to_copy._serverConfig;
		this->_request = to_copy._request; // Copie de ta partie Request
		this->_keepAlive = to_copy._keepAlive;

		this->isCGI = to_copy.isCGI;
		this->pipeType = to_copy.pipeType;
		this->ogFd = to_copy.ogFd;
		this->forkPid = to_copy.forkPid;
		this->cgiResponseBuff = to_copy.cgiResponseBuff;
	}
	return (*this);
}

Client::~Client() {}


int Client::getFd() const
{
	return (clientFd);
}

Request& Client::getRequest()
{
	return (_request);
}

const Config& Client::getConfig() const
{
	return (*_serverConfig);
}

time_t Client::getLastActivity() const
{
	return (_lastActivity);
}

void Client::updateActivity()
{
	_lastActivity = time(NULL);
}

void Client::resetClient()
{
	_request.reset(); 
	_keepAlive = true;
	_requestCount++;

	writeBuff.clear();
	buffSize = 0;
	bytesSent = 0;
	clientState = READING_REQUEST;
}

int Client::timeout()
{
	time_t current_time = time(NULL);
	double lastActSec = difftime(current_time, _lastActivity);
	
	if (clientState == READING_REQUEST && lastActSec >= KEEPALIVE_TIMEOUT)
		return (TIMEOUT);

	if (lastActSec >= IDLE_TIMEOUT)
		return (TIMEOUT);

	return (NO_TIMEOUT);
}