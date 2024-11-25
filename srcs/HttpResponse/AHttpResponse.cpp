/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:53:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/25 17:20:04 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <iostream>
#include "AHttpResponse.hpp"

AHttpResponse::~AHttpResponse() {
}

AHttpResponse::AHttpResponse(HttpRequest &request) : _request(request), _buffer_done(false), _header_ready(false), _status(0), _content_length(0) {}

AHttpResponse::AHttpResponse(const AHttpResponse &src) : _request(src._request) {
	*this = src;
}

AHttpResponse &AHttpResponse::operator=(const AHttpResponse &rhs) {
	if (this == &rhs)
		return (*this);
	this->_write_buffer = rhs._write_buffer;
	this->_buffer_done = rhs._buffer_done;
	this->_header_ready = rhs._header_ready;
	this->_status = rhs._status;
	this->_content_length = rhs._content_length;
	return (*this);
}

AHttpResponse &AHttpResponse::operator<<(const std::string &str)
{
	this->_write_buffer += str;
	return (*this);
}

IPollElement *AHttpResponse::getPollElement() {
	return (NULL);
}

int AHttpResponse::writeResponse(int fd) {
	int ret = 0;
	if (this->_write_buffer.empty())
		return (this->isBufferDone() ? 0 : 1);
	ret = send(fd, this->_write_buffer.c_str(), this->_write_buffer.size(), 0);
	if (ret == -1) {
		return (-1);
	}
	this->_content_length += ret;
	this->_write_buffer.erase(0, ret);
	return ((this->isBufferDone() && this->_write_buffer.empty()) ? 0 : 1);
}

HttpRequest &AHttpResponse::getRequest() {
	return (this->_request);
}

void AHttpResponse::setBufferDone(bool done) {
	this->_buffer_done = done;
}

bool AHttpResponse::isBufferDone() const {
	return (this->_buffer_done);
}

bool AHttpResponse::isHeaderReady() const
{
	return _header_ready;
}

uint16_t AHttpResponse::getStatus() const {
	return this->_status;
}

size_t AHttpResponse::getContentLength() const {
	return this->_content_length;
}

void	AHttpResponse::createHeaderBuffer(uint16_t code, const std::map<std::string, std::string> &headers){
	std::stringstream ss;
	ss << "HTTP/1.1 " << code << " " << this->getReasonPhrase(code) << "\r\n";
	std::string header = ss.str();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
		header += it->first + ": " + it->second + "\r\n";
	header += "\r\n";
	this->_write_buffer = header;
	this->_header_ready = true;
	this->_status = code;
}

std::string AHttpResponse::getReasonPhrase(uint16_t code) const {
	switch(code) {
		case 200:
			return "OK";
			break;
		case 400:
			return "Bad Request";
			break;
		case 403:
			return "Forbidden";
			break;
		case 404:
			return "Not Found";
			break;
		case 405:
			return "Method Not Allowed";
			break;
		case 406:
			return "Not Acceptable";
			break;
		case 411:
			return "Length Required";
			break;
		case 413:
			return "Payload Too Large";
			break;
		case 500:
			return "Internal Server Error";
			break;
		case 501:
			return "Not Implemented";
			break;
		default:
			return "Unknown Error";
			break;
	}
}

void AHttpResponse::appendBody(char * data, size_t size)
{
	this->_write_buffer.append(data, size);
}

void AHttpResponse::appendBody(const std::string & data)
{
	this->_write_buffer.append(data);
}
