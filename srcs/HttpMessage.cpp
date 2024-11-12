/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:39:29 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 18:10:34 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

HttpMessage::HttpMessage(void) {}

HttpMessage::HttpMessage(const HttpMessage &src) {
	*this = src;
}

HttpMessage::~HttpMessage(void) {}

HttpMessage	&HttpMessage::operator=(const HttpMessage &rhs) {
	if (this == &rhs)
		return (*this);
	this->_headers = rhs._headers;
	this->_body = rhs._body;
	return (*this);
}

std::map<std::string, std::string>	&HttpMessage::getHeaders(void) {
	return (this->_headers);
}

std::string HttpMessage::getHeader(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);

	if (it == this->_headers.end())
		return ("");
	return (it->second);
}

const std::string	&HttpMessage::getBody(void) const {
	return (this->_body);
}

void		HttpMessage::setHeader(const std::string &key, const std::string &value) {
	this->_headers[key] = value;
}

void		HttpMessage::setBody(const std::string &body) {
	this->_body = body;
}
