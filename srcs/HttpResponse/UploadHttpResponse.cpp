/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHttpResponse.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 14:56:41 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/18 16:39:39 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "UploadHttpResponse.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

UploadHttpResponse::UploadHttpResponse(HttpRequest &request, const std::string &path) : AHttpResponse(request) {
	this->_fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (!this->_fd)
		throw std::runtime_error("Failed to open file: " + std::string(std::strerror(errno)) + ": " + path);
	if (fcntl(this->_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set file descriptor to non-blocking: " + std::string(std::strerror(errno)));
	this->createHeaderBuffer(201, std::map<std::string, std::string>());
}


UploadHttpResponse::~UploadHttpResponse() {}

UploadHttpResponse::UploadHttpResponse(UploadHttpResponse const &src) : AHttpResponse(src) {
	*this = src;
}

UploadHttpResponse &UploadHttpResponse::operator=(UploadHttpResponse const & rhs) {
	if (this == &rhs)
		return (*this);
	AHttpResponse::operator=(rhs);
	this->_fd = rhs._fd;
	return (*this);
}

IPollElement *UploadHttpResponse::getPollElement() {
	return this;
}

int UploadHttpResponse::getFd() const {
	return this->_fd;
}

short UploadHttpResponse::getEvents() const {
	return POLLOUT;
}

int UploadHttpResponse::update(struct pollfd &pollfd, Configuration &config) {
	(void)config;
	if (pollfd.events & POLLOUT)
	{
		int ret = write(this->_fd, this->getRequest().getBodyBuffer().c_str(), this->getRequest().getBodyBuffer().size());
		if (ret == -1)
			return -1;
		if (ret != 0)
			this->getRequest().getBodyBuffer().erase(0, ret);
		if (this->getRequest().isDone() && this->getRequest().getBodyBuffer().empty())
		{
			this->setBufferDone(true);
		}
	}
	if (pollfd.events & POLLERR || pollfd.events & POLLHUP)
		this->setBufferDone(true);
	return 1;
}

