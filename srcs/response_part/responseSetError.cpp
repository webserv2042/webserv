#include "../../include/Response.hpp"
#include "../../include/Request.hpp"

std::map<e_status_code, std::string> Response::_statusMessage = initMessageStatus();

const char* repException::what() const throw()
{
    return ("Error detected");
}

void Response::fail(e_status_code code)
{
    _statusCode = code;
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
    code[static_cast<e_status_code>(409)] = "Conflict";
    code[static_cast<e_status_code>(413)] = "Content Too Large";
    code[static_cast<e_status_code>(414)] = "URI Too Long";
    code[static_cast<e_status_code>(500)] = "Internal Server Error";
    code[static_cast<e_status_code>(501)] = "Not Implemented";
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

    std::stringstream ssPath;
    ssPath << PATH_ERROR_PAGES << code << ".html";
    std::string path = ssPath.str();

    if (checkFile(path))
    {
        std::string content = readFile(path);
        _body.assign(content.begin(), content.end());
    }
    else
    {
        std::stringstream ss;
        ss << "<html><head><title>" << code << " " << message << "</title></head>";
        ss << "<body style='text-align:center; font-family:sans-serif; padding-top:100px;'>";
        ss << "<h1 style='font-size: 50px;'>" << code << "</h1>";
        ss << "<h2>" << message << "</h2>";
        ss << "<hr><p>Webserv/1.0 (Senshy Edition - Fallback Mode)</p></body></html>";
        
        std::string html = ss.str();
        _body.assign(html.begin(), html.end());
    }

    this->addHeaders("content-type", "text/html");
}

bool    Response::checkFile(const std::string &path)
{
    struct stat data;
    
    if (stat(path.c_str(), &data) == 0)
        return S_ISREG(data.st_mode);

    return false;
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
