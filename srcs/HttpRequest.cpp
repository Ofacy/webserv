/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:39:50 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/25 21:32:02 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "HttpRequest.hpp"
#include "BodyParser.hpp"
#include "ChunkedBodyParser.hpp"
#include <cerrno>
#include <iostream>
#include <cstdlib>

HttpRequest::HttpRequest(Configuration &config) : HttpMessage(), _config(config), _state(REQUEST_LINE), _body_parser(NULL), _content_length(0) {}

HttpRequest::HttpRequest(const HttpRequest &src) : HttpMessage(src), _config(src._config), _state(REQUEST_LINE), _body_parser(NULL), _content_length(0) {
	*this = src;
}

HttpRequest::~HttpRequest() {
	if (this->_body_parser) {
		if (this->_body_parser->getPollElement())
			this->_config.removePollElement(this->_body_parser->getPollElement());
		else delete this->_body_parser;
	}
}


HttpRequest	&HttpRequest::operator=(const HttpRequest &rhs) {
	if (this == &rhs)
		return (*this);
	this->_method = rhs._method;
	this->_uri = rhs._uri;
	this->_version = rhs._version;
	this->_state = rhs._state;
	this->_max_body_size = rhs._max_body_size;
	this->_content_length = rhs._content_length;
	this->_body_buffer = rhs._body_buffer;
	this->_buffer = rhs._buffer;
	return (*this);
}

const std::string	&HttpRequest::getVersion(void) const {
	return (this->_version);
}

const std::string	&HttpRequest::getMethod(void) const {
	return (this->_method);
}

const std::string	&HttpRequest::getUri(void) const {
	return (this->_uri);
}

size_t	HttpRequest::getContentLength(void) const {
	return (this->_content_length);
}

void	HttpRequest::setContentLength(size_t content_length) {
	this->_content_length = content_length;
}

size_t	HttpRequest::getMaxBodySize(void) const {
	return (this->_max_body_size);
}

void HttpRequest::setMaxBodySize(size_t max_body_size) {
	this->_max_body_size = max_body_size;
}

HttpRequest::HttpRequestState	HttpRequest::getState(void) const {
	return (this->_state);
}

void	HttpRequest::setState(HttpRequestState state) {
	this->_state = state;
}

bool	HttpRequest::isDone(void) const {
	return (this->getState() == DONE || this->getState() == INVALID);
}

bool	HttpRequest::isHeaderDone(void) const {
	return (this->getState() == DONE || this->getState() == BODY || this->getState() == INVALID || this->getState() == URI_TOO_LONG);
}

void	HttpRequest::update(char *buffer, size_t size) {
	if (this->getState() == BODY || this->getState() == PARSING_BODY)
	{
		this->_body_parser->update(buffer, size);
		return ;
	}
	this->_buffer.append(buffer, size);
	std::string line;
	size_t pos;
	while ((pos = this->_buffer.find("\r\n")) != std::string::npos) {
		line = this->_buffer.substr(0, pos);
		this->_parseLine(line);
		this->_buffer.erase(0, pos + 2);
		if (this->getState() == BODY)
		{
			try {
				this->_body_parser = this->_selectBodyParser();
				if (this->_body_parser->getPollElement())
					this->_config.addPollElement(this->_body_parser->getPollElement());
			}
			catch(std::exception &e) {
				this->setState(INVALID);
				std::cerr << "Failed to select body parser: " << e.what() << std::endl;
				return ;
			}
			this->_body_parser->update(this->_buffer.c_str(), this->_buffer.size());
			this->_buffer.clear();
			return ;
		}
	}
	if (this->_buffer.size() > this->_config.getClientMaxHeaderSize())
		this->setState(this->_state == REQUEST_LINE ? URI_TOO_LONG : INVALID);
}

std::string &HttpRequest::getBodyBuffer(void) {
	return (this->_body_buffer);
}

void	HttpRequest::_parseLine(std::string &line) {
	if (this->getState() == REQUEST_LINE)
		this->_parseRequestLine(line);
	else if (this->getState() == HEADERS)
	{
		if (line.empty())
		{
			this->setState(this->hasBody() ? BODY : DONE);
			return ;
		}
		this->_parseHeaderLine(line);
	}
}


// The Request-Line begins with a method token, followed by the
// Request-URI and the protocol version, and ending with CRLF. The
// elements are separated by SP characters. No CR or LF is allowed
// except in the final CRLF sequence.

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

void	HttpRequest::_parseRequestLine(std::string &line) {
	if (line.find("\r") != std::string::npos || line.find("\n") != std::string::npos) {
		this->setState(INVALID);
		return ;
	}
	
	size_t pos = line.find(' ');
	while ((pos = line.find(' ')) != std::string::npos)
	{
		if (this->_method.empty())
			this->_method = line.substr(0, pos);
		else if (this->_uri.empty())
			this->_uri = line.substr(0, pos);
		line.erase(0, pos + 1);
	}
	this->_version = line;
	if (this->_method.empty() || this->_uri.empty() || this->_version.empty() || this->_version.find(" ") != std::string::npos) {
		this->setState(INVALID);
		return ;
	}
	this->setState(HEADERS);
	if (this->_version != "HTTP/1.1") {
		this->setState(INVALID);
		return ;
	}
}


// Each header field consists of a name followed by a colon (":") and the field value.
// Field names are case-insensitive. The field value MAY be preceded by any amount
// of LWS, though a single SP is preferred. Header fields can be
// extended over multiple lines by preceding each extra line with at
// least one SP or HT.

// Header fields can be
// extended over multiple lines by preceding each extra line with at
// least one SP or HT.

void	HttpRequest::_parseHeaderLine(std::string &line) {
	size_t pos = line.find(':');
	if (pos == std::string::npos) {
		this->setState(INVALID);
		return ;
	}
	std::string key = line.substr(0, pos);
	if (key.find_first_not_of(" \t") == std::string::npos) {
		this->setState(INVALID);
		return ;
	}
	size_t value_pos = line.find_first_not_of(" \t", pos + 1);
	if (value_pos == std::string::npos)
		value_pos = 0;
	std::string value = line.substr(value_pos);
	this->setHeader(key, value);
}

BodyParser * HttpRequest::_selectBodyParser(void) {
	if (this->getHeader("Transfer-Encoding") == "chunked")
		return new ChunkedBodyParser(*this);
	else if (!this->getHeader("Content-Length").empty())
		return new BodyParser(*this);
	return NULL;
}


bool	HttpRequest::hasBody(void) const {
	return (!this->getHeader("Content-Length").empty() || this->getHeader("Transfer-Encoding") == "chunked");
}