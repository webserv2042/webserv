#ifndef ERRORS_HPP
# define ERRORS_HPP

#include <vector>

enum	e_status_code
{
	OK = 200,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	URI_TOO_LONG = 414,
	CONTENT_TOO_LARGE = 413,
	VERSION_NOT_SUPPORTED = 505
};

#endif