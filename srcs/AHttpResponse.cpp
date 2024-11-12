/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:53:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/12 19:09:33 by bwisniew         ###   ########.fr       */
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