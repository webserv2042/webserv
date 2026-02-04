#include "../../includes/config/Config.hpp"

Config::Config()
{
	_port = -1;
	_clientMaxBodySize = 0;
	_autoIndex = false;
}

int Config::getPort() const
{
	return (_port);
}

size_t Config::getClientMaxBodySize() const
{
	return (_clientMaxBodySize);
}

bool Config::getAutoIndex() const
{
	return (_autoIndex);
}

const std::vector<std::string>& Config::getServerNames() const
{
	return (_serverName);
}

const std::string& Config::getServerName(size_t index) const
{
	if (index >= _serverName.size())
		throw std::out_of_range("Index serveur invalide");
	return (_serverName[index]);
}

const std::string& Config::getHost() const
{
	return (_host);
}

const std::string& Config::getRoot() const
{
	return (_root);
}

const std::string& Config::getIndex() const
{
	return (_index);
}

const std::string& Config::getUploadPath() const
{
	return (_uploadPath);
}

const std::map<int, std::string>& Config::getErrorPage() const
{
	return (_errorPage);
}

const std::vector<std::string>& Config::getAllowedMethods() const
{
	return (_allowedMethods);
}

const std::vector<Location>& Config::getLocations() const
{
	return (_locations);
}

void Config::setPort(int value)
{
	_port = value;
}

void Config::setClientMaxBodySize(size_t value)
{
	_clientMaxBodySize = value;
}

void Config::setHost(const std::string& value)
{
	_host = value;
}

void Config::setRoot(const std::string& value)
{
	_root = value;
}

void Config::setIndex(const std::string& value)
{
	_index = value;
}

void Config::setUploadPath(const std::string& value)
{
	_uploadPath = value;
}

void Config::setAutoIndex(bool value)
{
	_autoIndex = value;
}

void Config::addServerName(const std::string& name)
{
	_serverName.push_back(name);
}

void Config::addErrorPage(int code, const std::string& path)
{
	_errorPage[code] = path;
}

void Config::addAllowedMethod(const std::string& method)
{
	_allowedMethods.push_back(method);
}

void Config::addLocation(const Location& location)
{
	_locations.push_back(location);
}
