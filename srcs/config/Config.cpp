#include "../../includes/config/Config.hpp"

Config::Config()
{
	_port = -1;
	_clientMaxBodySize = 0;
	_serverName = "";
}

int Config::getPort() const
{
	return (_port);
}

size_t Config::getClientMaxBodySize() const
{
	return (_clientMaxBodySize);
}

std::string Config::getServerName() const
{
	return (_serverName);
}

void Config::setServerName(const std::string &name)
{
	_serverName = name;
}

void Config::setPort(int value)
{
	_port = value;
}

void Config::setClientMaxBodySize(size_t value)
{
	_clientMaxBodySize = value;
}


