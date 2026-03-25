#include "../include/Utils.hpp"

void    trim(std::string &line)
{
	size_t  start = line.find_first_not_of(" \t\r\n"); // je cherche avant la string
	size_t  end = line.find_last_not_of(" \t\r\n"); // on cherche après la string

	if (start == std::string::npos)
		line.clear();
	else
		line = line.substr(start, end - start + 1);
}

std::string toLower(std::string str)
{
    for (size_t i = 0; i < str.length(); ++i)
        str[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(str[i])));

    return (str);
}
