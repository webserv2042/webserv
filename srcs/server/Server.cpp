#include "../../includes/server/Server.hpp"
#include <errno.h>
#include <string.h>

// Server::Server(const Config& configServer) : membreRef(configServer) {};

Server::Server(const Config& conf) : _serverConfig(conf)
{
}

//COPY CONSTRUCTOR
Server::Server(const Server& to_copy) {
	*this = to_copy;
}

//COPY ASSIGNMENT
Server& Server::operator=(const Server& to_copy) {
	if (this != &to_copy)
	{
		_serverConfig = to_copy._serverConfig;
		_socketFD = to_copy._socketFD;
		_socketAddress = to_copy._socketAddress; // Carte d'identité
		_socketAddressLength = to_copy._socketAddressLength;
	}
	return (*this);
}

Server::~Server()
{
}

void Server::init()
{
	this->_socketFD = socket(IPV4, SOCK_STREAM, 0);
	if (this->_socketFD == -1)
		throw std::runtime_error("(SERVER) Failed to initialize socket");
	int flags = fcntl(this->_socketFD, F_GETFL);
	if (flags == -1)
		throw std::runtime_error("(SERVER) Failed to get socket configuration");
	flags = flags | O_NONBLOCK;

	//! reuse address necessary
	int opt = 1;
	setsockopt(this->_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	fcntl(this->_socketFD, F_SETFL, flags);
	this->_socketAddress.sin_family = IPV4;
	this->_socketAddress.sin_port = htons(_serverConfig.getPort()); // htons = host to network short
	this->_socketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // htonl = host to network long
	this->_socketAddressLength = sizeof(this->_socketAddress);
	int bindReturnCode = bind(_socketFD, reinterpret_cast<struct sockaddr*> (&this->_socketAddress), this->_socketAddressLength);
	if (bindReturnCode == -1)
		throw std::runtime_error("(SERVER) Failed to link socket");
	int listenReturnCode = listen(_socketFD, PENDING_QUEUE_MAXLENGTH);

	if (listenReturnCode == -1)
		throw std::runtime_error("(SERVER) Failed to start listening");
}

int Server::getSocketAddressLength() const
{
	return (_socketAddressLength);
}

int Server::getSocketFD() const
{
	return (_socketFD);
}

struct sockaddr_in Server::getSocketAdress() const
{
	return (_socketAddress);
}

void Server::setSocketFD(int value)
{
	_socketFD = value;
}

void Server::setSocketAddressLength(int value)
{
	_socketAddressLength = value;
}

void Server::setSocketAddress(const sockaddr_in &address)
{
	_socketAddress = address;
}
