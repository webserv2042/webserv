#include "../../includes/server/Server.hpp"

Server::Server(int port)
{
	this->port = port;
	this->socketFD = -1;
}

void Server::init()
{
	this->socketFD = socket(IPV4, SOCK_STREAM, 0);
	if (this->socketFD == -1)
		throw std::runtime_error("(SERVER) Failed to initialize socket");
	int flags = fcntl(this->socketFD, F_GETFL);
	if (flags == -1)
		throw std::runtime_error("(SERVER) Failed to get socket configuration");
	flags = flags | O_NONBLOCK;
	fcntl(this->socketFD, F_SETFL, flags);
	this->socketAddress.sin_family = IPV4;
	this->socketAddress.sin_port = htons(LISTENING_PORT); // htons = host to network short
	this->socketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // htonl = host to network long
	this->socketAddressLength = sizeof(this->socketAddress);
	int bindReturnCode = bind(socketFD, (struct sockaddr*) &this->socketAddress, this->socketAddressLength);
	if (bindReturnCode == -1)
		throw std::runtime_error("(SERVER) Failed to link socket");
	int listenReturnCode = listen(socketFD, PENDING_QUEUE_MAXLENGTH);
	if (listenReturnCode == -1)
		throw std::runtime_error("(SERVER) Failed to start listening");
}
