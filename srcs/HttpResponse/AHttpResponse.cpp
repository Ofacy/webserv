/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:53:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/15 19:02:54 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <iostream>
#include "AHttpResponse.hpp"


AHttpResponse::~AHttpResponse() {
}

AHttpResponse::AHttpResponse(HttpRequest &request) : _request(request), _buffer_done(false), _header_ready(false) {}

AHttpResponse::AHttpResponse(const AHttpResponse &src) : _request(src._request) {
	*this = src;
}

AHttpResponse &AHttpResponse::operator=(const AHttpResponse &rhs) {
	if (this == &rhs)
		return (*this);
	this->_write_buffer = rhs._write_buffer;
	this->_buffer_done = rhs._buffer_done;
	this->_header_ready = rhs._header_ready;
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
	//std::cout << "Sent " << ret << " bytes" << std::endl;
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

void	AHttpResponse::createHeaderBuffer(uint16_t code, std::map<std::string, std::string> &headers){
	std::stringstream ss;
	ss << "HTTP/1.1 " << code << " " << this->getReasonPhrase(code) << "\r\n";
	std::string header = ss.str();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		header += it->first + ": " + it->second + "\r\n";
	header += "\r\n";
	this->_write_buffer = header;
	this->_header_ready = true;
	// std::cout << "Responding with: " << code << " " << this->getReasonPhrase(code) << std::endl;
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
			return "";
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
