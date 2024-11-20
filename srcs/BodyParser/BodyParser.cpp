/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 15:57:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/20 19:21:39 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BodyParser.hpp"
#include <cerrno>
#include <cstdlib>

BodyParser::BodyParser(HttpRequest &request) : _request(request), _current_body_size(0) {
	char *endptr;
	size_t content_length;
	content_length = std::strtoul(this->getRequest().getHeader("Content-Length").c_str(), &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE)
	{
		throw std::runtime_error("Invalid Content-Length header");
	}
	this->getRequest().setContentLength(content_length);
}

BodyParser::BodyParser(const BodyParser &src) : _request(src._request), _current_body_size(0) {
	*this = src;
}

BodyParser &BodyParser::operator=(const BodyParser &src) {
	if (this == &src)
		return (*this);
	return (*this);
}

BodyParser::~BodyParser() {
}

IPollElement *BodyParser::getPollElement() {
	return NULL;
}

void	BodyParser::update(const char *data, size_t len) {
	if (this->_current_body_size + len > this->_request.getContentLength())
	{
		this->_request.setState(HttpRequest::INVALID);
		return ;
	}
	this->_request.getBodyBuffer().append(data, len);
	this->_current_body_size += len;
	if (this->_current_body_size == this->_request.getContentLength())
	{
		this->_request.setState(HttpRequest::DONE);
	}
}

HttpRequest	&BodyParser::getRequest() {
	return (this->_request);
}