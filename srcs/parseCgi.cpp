#include "../include/response.hpp"
#include "../include/request.hpp"

void    Response::parseCgi(const std::vector<char> &cgi)
{
	const std::string 	delimCRLF = "\r\n\r\n";

	std::vector<char>::const_iterator it = search(cgi.begin(), cgi.end(), delimCRLF.begin(), delimCRLF.end());

	if (it != cgi.end())
	{
		std::string			headers(cgi.begin(), it);
		std::stringstream	ss(headers);
		std::string			line;
		
		while (std::getline(ss, line))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (!line.empty())
				this->parseHeadersCgi(line);
		}
		_body.assign(it + 4, cgi.end());
	}
	else
		_body = cgi;
}

void    Response::parseHeadersCgi(const std::string &line)
{
	size_t  delim = line.find(':');
	
	if (delim == std::string::npos)
		return ;

	std::string key = line.substr(0, delim);
	std::string value = line.substr(delim + 1);

	trim(key);
	trim(value);

	key = toLower(key);
	
	if (key == "status")
   	{
		char* endPtr;
		long code = std::strtol(value.c_str(), &endPtr, 10);
		
		if (endPtr != value.c_str() && code >= 100 && code < 600) 
			this->_statusCode = static_cast<e_status_code>(code);
		else
			this->_statusCode = INTERNAL_SERVER_ERROR;
	}
	else
		this->addHeaders(key, value);
}

// void   trim(std::string &line)
// {
// 	size_t  start = line.find_first_not_of(" \t\r\n"); // je cherche avant la string
// 	size_t  end = line.find_last_not_of(" \t\r\n"); // on cherche après la string

// 	if (start == std::string::npos)
// 		line.clear();
// 	else
// 		line = line.substr(start, end - start + 1);
// }