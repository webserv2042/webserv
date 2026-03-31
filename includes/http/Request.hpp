#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <vector>
#include <algorithm>    
#include <string>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <sstream>

#include "Utils.hpp"
#include "Errors.hpp"
// #include "Config.hpp"

class Config;

#define MAX_BYTES	8000 // recommandé dans la RFC 9112 d'imposer cette limite
#define LIMIT_BODY	20971520 // 20 Mo pour le body

enum    e_request_method    
{
	GET,
	POST,
	DELETE,
	NOT_ALLOWED,
	DEFAULT
};

enum	e_chunk_step
{
	SEARCH_SIZE,
	READ_DATA
};

enum	e_parsing_steps
{
	START_LINE,
	HEADERS,
	BODY,
	TRAILERS,
	FINISHED
};

class Request
{
	private:
		std::vector<char>   						_bytesData;
		std::map<std::string, std::string>			_allHeaders;
		std::string         						_method;
		std::string         						_uri;
		std::string									_queryString;
		std::string         						_httpVersion;
		std::string         						_body;
		std::map<std::string, std::string>			_cookies;
		size_t										_contentLength;
		bool										_isContentLength;
		size_t										_chunkSize;
		e_request_method							_methodEnum;
		e_parsing_steps								_step;
		e_chunk_step								_chunkStep;
		e_status_code								_errorCode;

	public:
		Request();
		~Request();

		//******************** PARTIE 1 : Réception du client ******************//
		void                						feeding(const char *buffer, size_t sizeOfBytesRead);

		bool                						isFinished() const;
		void										setMethod(const std::string &method);

		std::vector<char>							getRequest() const;
		std::string         						getMethod() const;
		std::string         						getUri() const;
		std::string         						getHttpVersion() const;
		std::string									getBody() const;
		const std::map<std::string, std::string>	&getAllHeaders() const;
		std::string									getHeader(const std::string& key) const;
		std::string									getSupportedMethod() const;
		e_parsing_steps								getStep() const;
		e_status_code								getErrorCode() const;
		size_t										getContentLength() const;
		std::string									getQueryString() const;
		std::string									getCookie(const std::string& name) const;
		std::string									getCookiesMap() const;

		//******************** PARTIE 2 : Parsing **************************//
		void										parseRequest();
		void										parseLines();
		void										parseStartLine(const std::string &line);
		void										checkStartLine();
		void										parseHttp();
		std::string									decodeUri(const std::string &uri);
		std::string									parseUri(std::string uri);
		void										parseHeaders(const std::string &line);
		void										parseCookie(const std::string &data);
		void										parseBodyContent();
		void										parseBodyChunked();
		void										parseTrailers();
		void										reset();
		void										fail(e_status_code code);

		//Erreurs parsing
		bool										errorMaxBytes();
		void										printRequest() const;
};

#endif