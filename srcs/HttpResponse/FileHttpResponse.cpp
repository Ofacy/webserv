/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHttpResponse.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 18:28:08 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/26 17:59:26 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "FileHttpResponse.hpp"
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

FileHttpResponse::FileHttpResponse(HttpRequest &request, uint16_t status, int fd, struct stat stats) : AHttpResponse(request), _fd(fd)
{
	if (fcntl(this->_fd, F_SETFL, O_NONBLOCK) == -1) {
		this->~FileHttpResponse();
		throw std::runtime_error("Failed to set file descriptor to non-blocking: " + std::string(std::strerror(errno)));
	}
	std::map<std::string, std::string> headers;
	std::stringstream ss;
	ss << stats.st_size;
	headers["Content-Length"] = ss.str();
	this->createHeaderBuffer(status, headers);
	if (request.getMethod() == "HEAD") {
		this->setBufferDone(true);
		close(this->_fd);
		this->_fd = -1;
	}
}

FileHttpResponse::FileHttpResponse(const FileHttpResponse &src) : AHttpResponse(src)
{
	*this = src;
}

FileHttpResponse &FileHttpResponse::operator=(const FileHttpResponse &src)
{
	if (this == &src)
		return *this;
	AHttpResponse::operator=(src);
	this->_fd = src._fd;
	return *this;
}

FileHttpResponse::~FileHttpResponse()
{
	if (this->_fd != -1)
		close(this->_fd);
}

IPollElement *FileHttpResponse::getPollElement()
{
	if (this->_fd == -1)
		return NULL;
	return this;
}

int FileHttpResponse::getFd() const
{
	return this->_fd;
}

short FileHttpResponse::getEvents() const
{
	return POLLIN;
}

int FileHttpResponse::update(pollfd &pollfd, Configuration &config)
{
	(void)config;
	char	buffer[FILE_BUFFER_SIZE];

	if (pollfd.revents & POLLIN)
	{
		ssize_t read_bytes = read(this->_fd, buffer, FILE_BUFFER_SIZE);
		if (read_bytes == -1)
		{
			std::cerr << "Error reading file" << std::endl;
			this->setBufferDone(true);
			return 1;
		}
		if (read_bytes == 0)
		{
			this->setBufferDone(true);
			pollfd.events = 0;
		}
		this->appendBody(buffer, read_bytes);
	}
	else if (pollfd.revents & (POLLERR | POLLHUP))
	{
		std::cerr << "Error reading file" << std::endl;
		this->setBufferDone(true);
		return 1;
	}
	return 1;
}
