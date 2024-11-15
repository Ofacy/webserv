/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHttpResponse.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 18:28:08 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/15 16:58:23 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileHttpResponse.hpp"
#include <unistd.h>
#include <iostream>

FileHttpResponse::FileHttpResponse(HttpRequest &request, uint16_t status, int fd, struct stat stats) : AHttpResponse(request), _fd(fd)
{
	std::map<std::string, std::string> headers;
	std::stringstream ss;
	ss << stats.st_size;
	headers["Content-Length"] = ss.str();
	this->createHeaderBuffer(status, headers);
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
			return -1;
		}
		if (read_bytes == 0)
		{
			this->setBufferDone(true);
			::close(this->_fd);
			this->_fd = -1;
			pollfd.events = 0;
			pollfd.fd = -1;
		}
		this->appendBody(buffer, read_bytes);
	}
	else if (pollfd.revents & (POLLERR | POLLHUP))
	{
		std::cerr << "Error reading file" << std::endl;
		return -1;
	}
	return 1;
}
