#include "../../includes/http/Response.hpp"
#include "../../includes/http/Request.hpp"

std::map<e_status_code, std::string> Response::_statusMessage = initMessageStatus();

const char* repException::what() const throw()
{
    return ("Error detected");
}

void Response::fail(e_status_code code)
{
    _statusCode = code;
    if (code == BAD_REQUEST || code == CONTENT_TOO_LARGE)
        _closeFd = true;
    throw repException();
}

std::map<e_status_code, std::string> Response::initMessageStatus()
{
    std::map<e_status_code, std::string> code;

    code[static_cast<e_status_code>(200)] = "OK";
    code[static_cast<e_status_code>(201)] = "Created";
    code[static_cast<e_status_code>(204)] = "No Content";
    code[static_cast<e_status_code>(301)] = "Moved Permanently";
    code[static_cast<e_status_code>(302)] = "Found";
    code[static_cast<e_status_code>(400)] = "Bad Request";
    code[static_cast<e_status_code>(403)] = "Forbidden";
    code[static_cast<e_status_code>(404)] = "Not Found";
    code[static_cast<e_status_code>(405)] = "Method Not Allowed";
    code[static_cast<e_status_code>(408)] = "Request Timeout";
    code[static_cast<e_status_code>(409)] = "Conflict";
    code[static_cast<e_status_code>(413)] = "Content Too Large";
    code[static_cast<e_status_code>(414)] = "URI Too Long";
    code[static_cast<e_status_code>(431)] = "Request Headers Fields Too Large";
    code[static_cast<e_status_code>(500)] = "Internal Server Error";
    code[static_cast<e_status_code>(501)] = "Not Implemented";
    code[static_cast<e_status_code>(502)] = "Bad Gateway";
    code[static_cast<e_status_code>(504)] = "Gateway Timeout";
    code[static_cast<e_status_code>(505)] = "HTTP Version Not Supported";

    return (code);
}

void Response::generateErrorPage(e_status_code code)
{
    _statusCode = code;
    _body.clear();
    std::string message = _statusMessage[code];

    if (message.empty())
        message = "UNKNOWN ERROR";

    std::vector<std::string> pathConfig;

    if (_structLocation)
    {
        std::map<int, std::string>::const_iterator it = _structLocation->errorPage.find(static_cast<int>(code));
        if (it != _structLocation->errorPage.end())
        {
            std::string pathErrorInLoc = it->second;
            std::string root = _config.getRoot();

            if (!pathErrorInLoc.empty())
            {
                if (pathErrorInLoc[0] == '/')
                    pathErrorInLoc.erase(0, 1);

                if (!root.empty() && root[root.size() - 1] != '/')
                    root += "/";

                pathErrorInLoc = root + pathErrorInLoc;
            }
            pathConfig.push_back(pathErrorInLoc);
        }
    }
    {
        std::map<int, std::string>::const_iterator it = _config.getErrorPage().find(static_cast<int>(code));
        if (it != _config.getErrorPage().end())
        {
            std::string pathErrorInGlobal = it->second;
            std::string root = _config.getRoot();
            if (!pathErrorInGlobal.empty()) 
            {
                if (pathErrorInGlobal[0] == '/')
                    pathErrorInGlobal.erase(0, 1);

                if (!root.empty() && root[root.size() - 1] != '/')
                    root += "/";

                pathErrorInGlobal = root + pathErrorInGlobal;
            }
            pathConfig.push_back(pathErrorInGlobal);
        }
    }

    std::stringstream ssPath;
    ssPath << PATH_ERROR_PAGES << code << ".html";
    pathConfig.push_back(ssPath.str());

    bool success = false;
   for (size_t i = 0; i < pathConfig.size(); ++i)
    {
        bool exists = checkFile(pathConfig[i]);
        std::cout << "[TRY " << i << "] Path: " << pathConfig[i] 
                << (exists ? " -> \033[32mFOUND\033[0m" : " -> \033[31mNOT FOUND\033[0m") 
                << std::endl;

        if (exists)
        {
            std::string content = readFile(pathConfig[i]);
            if (!content.empty())
            {
                _body.assign(content.begin(), content.end());
                success = true;
                break;
            }
        }
    }

    if (!success)
    {
        std::stringstream ss;
        ss << "<html><head><title>" << code << " " << message << "</title></head>";
        ss << "<body style='text-align:center; font-family:sans-serif; padding-top:100px;'>";
        ss << "<h1 style='font-size: 50px;'>" << code << "</h1>";
        ss << "<h2>" << message << "</h2>";
        ss << "<hr><p>Webserv/1.0 (Fallback Mode)</p></body></html>";

        std::string html = ss.str();
        _body.assign(html.begin(), html.end());
    }

    this->addHeaders("content-type", "text/html");
}

bool    Response::checkFile(const std::string &path)
{
    struct stat data;
    
    if (stat(path.c_str(), &data) != 0) // erreur 
        return (false);

    if (!S_ISREG(data.st_mode)) // dossier
        return (false);

    if (access(path.c_str(), R_OK) != 0) // lecture interdite
        return (false);

    return (true);
}

std::string Response::readFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::binary);

    if (!file.is_open())
        return ("");
    
    std::stringstream ss;
    
    ss << file.rdbuf();
    return (ss.str());
}

// void Response::resetAndSetupBasics()
// {
//     _headers.clear();
//     _body.clear();
    
//     this->setHttpDate();
//     this->addHeaders("server", "Webserv/1.0");
// }
