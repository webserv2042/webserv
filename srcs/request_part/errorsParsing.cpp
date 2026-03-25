#include "../../include/Request.hpp"

bool    Request::errorMaxBytes()
{
    if (_bytesData.size() > MAX_BYTES )
	{
        _errorCode = (_step == START_LINE) ? URI_TOO_LONG : CONTENT_TOO_LARGE;
        throw repException();
    }
    return (false) ;
}

void    Request::fail(e_status_code code)
{
    _errorCode = code;     // On enregistre la cause (ex: 400, 413, 505)
    _step = FINISHED;      // On force l'arrêt de l'automate
    _bytesData.clear();    // Optionnel : on vide le buffer pour ne pas traiter de débris
    throw std::exception(); // On déclenche l'arrêt immédiat de l'exécution
}