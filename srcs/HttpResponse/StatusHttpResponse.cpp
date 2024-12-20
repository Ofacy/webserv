/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusHttpResponse.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:44:11 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/25 23:30:03 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "StatusHttpResponse.hpp"
#include <sstream>

StatusHttpResponse::StatusHttpResponse(HttpRequest &request, uint16_t code, const std::string &body) : AHttpResponse(request) {
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = "text/html";
	if (code >= 300 && code < 400)
		headers["Location"] = body;
	else {
		std::stringstream ss;
		ss << body.size();
		headers["Content-Length"] = ss.str();
	}
	this->createHeaderBuffer(code, headers);
	this->setBufferDone(true);
	if (request.getMethod() != "HEAD" && (code < 300 || code >= 400))
		*this << body;
}

StatusHttpResponse::StatusHttpResponse(HttpRequest &request, uint16_t code) : AHttpResponse(request) {
	std::map<std::string, std::string> headers;
	std::stringstream ss;
	std::stringstream code_ss;
	code_ss << code;
	std::string body(STATUS_PAGE);
	body.replace(body.find("#title#"), 7, code_ss.str() + " - " + this->getReasonPhrase(code));
	body.replace(body.find("#code#"), 6, code_ss.str());
	body.replace(body.find("#message#"), 9, this->getReasonPhrase(code));
	ss << body.size();
	headers["Content-Type"] = "text/html";
	headers["Content-Length"] = ss.str();
	this->createHeaderBuffer(code, headers);
	if (request.getMethod() != "HEAD")
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

