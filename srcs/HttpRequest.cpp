/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:39:50 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/15 17:38:57 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <iostream>

HttpRequest::HttpRequest() : HttpMessage(), _state(REQUEST_LINE) {}

HttpRequest::HttpRequest(const HttpRequest &src) : HttpMessage(src), _state(REQUEST_LINE) {
	*this = src;
}

HttpRequest::~HttpRequest() {}

HttpRequest	&HttpRequest::operator=(const HttpRequest &rhs) {
	if (this == &rhs)
		return (*this);
	this->_method = rhs._method;
	this->_uri = rhs._uri;
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

HttpRequest::HttpRequestState	HttpRequest::getState(void) const {
	return (this->_state);
}

bool	HttpRequest::isDone(void) const {
	return (this->_state == DONE || this->_state == INVALID);
}

bool	HttpRequest::isHeaderDone(void) const {
	return (this->_state == DONE || this->_state == BODY || this->_state == INVALID);
}

void	HttpRequest::update(char *buffer, size_t size) {
	if (size == 0 && this->_state == BODY)
	{
		this->_state = DONE;
		return;
	}
	this->_buffer.append(buffer, size);
	std::string line;
	size_t pos;
	while ((pos = this->_buffer.find("\r\n")) != std::string::npos) {
		line = this->_buffer.substr(0, pos);
		this->_parseLine(line);
		this->_buffer.erase(0, pos + 2);
	}
	return;
}

std::string &HttpRequest::getBodyBuffer(void) {
	return (this->_body_buffer);
}

void	HttpRequest::_parseLine(std::string &line) {
	if (this->_state == REQUEST_LINE)
		this->_parseRequestLine(line);
	else if (this->_state == HEADERS)
	{
		if (line.empty())
		{
			this->_state = this->hasBody() ? BODY : DONE;
			return ;
		}
		this->_parseHeaderLine(line);
	}
	else if (this->_state == BODY)
	{
		if (line.empty())
		{
			this->_state = DONE;
			return ;
		}
		this->_body_buffer.append(line); // TODO: handle chunked encoding
	}
}


// The Request-Line begins with a method token, followed by the
// Request-URI and the protocol version, and ending with CRLF. The
// elements are separated by SP characters. No CR or LF is allowed
// except in the final CRLF sequence.

// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

void	HttpRequest::_parseRequestLine(std::string &line) {
	if (line.find("\r") != std::string::npos || line.find("\n") != std::string::npos) {
		this->_state = INVALID;
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
		this->_state = INVALID;
		return ;
	}
	std::cout << "========== " << std::endl;
	std::cout << "Method: " << this->_method << std::endl;
	std::cout << "URI: " << this->_uri << std::endl;
	std::cout << "Version: " << this->_version << std::endl;
	std::cout << "========== " << std::endl;
	this->_state = HEADERS;
	if (this->_version != "HTTP/1.1") {
		this->_state = INVALID;
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
		this->_state = INVALID;
		return ;
	}
	std::string key = line.substr(0, pos);
	if (key.find_first_not_of(" \t") == std::string::npos) {
		this->_state = INVALID;
		return ;
	}
	size_t value_pos = line.find_first_not_of(" \t", pos + 1);
	if (value_pos == std::string::npos)
		value_pos = 0;
	std::string value = line.substr(value_pos);
	//std::cout << "Header : " << key << ": " << value << std::endl;
	this->setHeader(key, value);
}

bool	HttpRequest::hasBody(void) const {
	return (!this->getHeader("Content-Length").empty() || this->getHeader("Transfer-Encoding") == "chunked");
}