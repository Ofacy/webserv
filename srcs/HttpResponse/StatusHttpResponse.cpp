/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusHttpResponse.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:44:11 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/13 17:16:45 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "StatusHttpResponse.hpp"

StatusHttpResponse::StatusHttpResponse(HttpRequest &request, uint16_t code) : AHttpResponse(request) {
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = "text/html";
	this->createHeaderBuffer(code, headers);
	*this << this->getReasonPhrase(code);
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

