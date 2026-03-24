#ifndef ERRORS_HPP
# define ERRORS_HPP

#include <vector>
#include <exception>

enum	e_status_code
{
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	CONFLICT = 409,
	URI_TOO_LONG = 414,
	CONTENT_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	VERSION_NOT_SUPPORTED = 505
};

class repException : public std::exception
{
	public:
    	virtual const char* what() const throw();
};

#endif