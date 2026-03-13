#include "../include/request.hpp"

bool    Request::errorMaxBytes()
{
    if (_bytesData.size() > MAX_BYTES )
	{
        _errorCode = (_step == START_LINE) ? URI_TOO_LONG : BAD_REQUEST;
        throw std::exception();
    }
    return false ;
}