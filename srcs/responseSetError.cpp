#include "../include/response.hpp"

static std::map<int, std::string> initMessageStatus()
{
    std::map<int, std::string> code;

    code[200] = "OK";
    code[400] = "BAD_REQUEST";
    code[404] = "NOT_FOUND";
    code[405] = "METHOD_NOT_ALLOWED";
    code[414] = "URI_TOO_LONG";
    code[413] = "CONTENT_TOO_LARGE";
    code[505] = "VERSION_NOT_SUPPORTED";

    return (code);
}

std::map<int, std::string> Response::_statusMessage = initMessageStatus();