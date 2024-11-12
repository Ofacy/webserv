/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:53:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/12 19:26:11 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AHttpResponse.hpp"


AHttpResponse::~AHttpResponse() {}

AHttpResponse::AHttpResponse(HttpRequest &request) : _request(request) {}

AHttpResponse::AHttpResponse(const AHttpResponse &src) : _request(src._request) {
	*this = src;
}

AHttpResponse &AHttpResponse::operator=(const AHttpResponse &rhs) {
	if (this == &rhs)
		return (*this);
	this->_write_buffer = rhs._write_buffer;
	return (*this);
}


IPollElement *AHttpResponse::getPollElement() {
	return (nullptr);
}

int AHttpResponse::writeResponse(int fd) {
	int ret = 0;
	
	if (this->_write_buffer.empty())
		return (this->isBufferDone() ? 0 : 1);
	ret = send(fd, this->_write_buffer.c_str(), this->_write_buffer.size(), 0);
	if (ret == -1) {
		return (-1);
	}
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
