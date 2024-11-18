/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusHttpResponse.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:44:11 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/18 20:38:54 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "StatusHttpResponse.hpp"
#include <sstream>

StatusHttpResponse::StatusHttpResponse(HttpRequest &request, uint16_t code, const std::string &body) : AHttpResponse(request) {
	std::map<std::string, std::string> headers;
	std::stringstream ss;
	ss << body.size();
	headers["Content-Type"] = "text/html";
	headers["Content-Length"] = ss.str();
	this->createHeaderBuffer(code, headers);
	*this << body;
	this->setBufferDone(true);
}

StatusHttpResponse::StatusHttpResponse(HttpRequest &request, uint16_t code) : AHttpResponse(request) {
	std::map<std::string, std::string> headers;
	std::stringstream ss;
	std::string body = this->getReasonPhrase(code);
	ss << body.size();
	headers["Content-Type"] = "text/html";
	headers["Content-Length"] = ss.str();
	this->createHeaderBuffer(code, headers);
	*this << body;
	this->setBufferDone(true);
}

StatusHttpResponse::StatusHttpResponse(const StatusHttpResponse &src) : AHttpResponse(src) {
	*this = src;
}

StatusHttpResponse::~StatusHttpResponse(void)
{
}

StatusHttpResponse &StatusHttpResponse::operator=(const StatusHttpResponse &rhs) {
	if (this == &rhs)
		return (*this);
	return (*this);
}

