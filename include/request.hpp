#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <vector>
#include <algorithm>    
#include <string>
#include <cstdlib>
#include <map>
#include <sstream>

#include "errors.hpp"

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
		std::vector<char>   						_bytesData; // tab dynamique des octets que je reçois
		std::map<std::string, std::string>			_allHeaders;
		std::string         						_method;
		std::string         						_uri;
		std::string									_queryString;
		std::string         						_httpVersion;
		std::string         						_body;
		size_t										_contentLength;
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

		//******************** PARTIE 2 : Parsing **************************//
		void										parseRequest();
		void										parseLines();
		void										parseStartLine(const std::string &line);
		void										checkStartLine();
		void										parseHeaders(const std::string &line);
		void										parseBodyContent();
		void										parseBodyChunked();
		void										parseTrailers();
		void										trim(std::string &line);

		//Erreurs parsing
		bool										errorMaxBytes();
};

#endif