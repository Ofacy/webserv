/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedBodyParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 16:01:10 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/22 21:40:31 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "ChunkedBodyParser.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

ChunkedBodyParser::ChunkedBodyParser(HttpRequest &request) : BodyParser(request), _state(READ_CHUNK_SIZE) {
	std::stringstream ss;
	ss << "/tmp/" << std::clock() << ".webserv";
	this->_filename = ss.str();
	std::cout << "Creating file " << this->_filename << std::endl;
	this->_fd = open(this->_filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_NONBLOCK, S_IRUSR | S_IWUSR);
	if (this->_fd == -1)
		throw std::runtime_error("Failed to open file");
	this->getRequest().setState(HttpRequest::PARSING_BODY);
}

ChunkedBodyParser::ChunkedBodyParser(const ChunkedBodyParser &src) : BodyParser(src) {
	*this = src;
}

ChunkedBodyParser &ChunkedBodyParser::operator=(const ChunkedBodyParser &src) {
	if (this == &src)
		return (*this);
	BodyParser::operator=(src);
	this->_fd = src._fd;
	this->_state = src._state;
	this->_chunk_size = src._chunk_size;
	this->_buffer = src._buffer;
	this->_filename = src._filename;
	return (*this);
}

ChunkedBodyParser::~ChunkedBodyParser() {
	close(this->_fd);
	std::remove(this->_filename.c_str());
}

void	ChunkedBodyParser::update(const char *data, size_t len) {
	this->_buffer.append(data, len);
	while (this->_state != WRITE_RESPONSE && this->_state != INVALID && !this->_buffer.empty()) {
		if (this->_state == READ_CHUNK_SIZE) {
			if (this->_readChunkSize() != 0)
				break;
		}
		if (this->_state == READ_CHUNK) {
			if (this->_readChunk() != 0)
				break;
		}
		if (this->_state == READ_CHUNK_END) {
			if (this->_readChunkEnd() != 0)
				break;
		}
	}
}

IPollElement *ChunkedBodyParser::getPollElement() {
	return this;
}

int ChunkedBodyParser::getFd() const {
	return this->_fd;
}

short ChunkedBodyParser::getEvents() const {
	if (this->_state == WRITE_RESPONSE)
		return (POLLIN);
	return (POLLOUT);
}

int	ChunkedBodyParser::update(struct pollfd &pollfd, Configuration &config) {
	(void)config;
	if (pollfd.revents & POLLOUT)
		this->_writeFile(pollfd);
	if (pollfd.revents & POLLIN)
		this->_readFile(pollfd);
	if (pollfd.revents & (POLLHUP | POLLERR) && this->_state != WRITE_RESPONSE)
		this->getRequest().setState(HttpRequest::DONE);
	return (1);
}

void ChunkedBodyParser::_writeFile(struct pollfd &pollfd) {
	ssize_t written = write(this->_fd, this->_file_buffer.c_str(), this->_file_buffer.size());
	if (written == -1)
		throw std::runtime_error(std::string("Failed to write to file") + std::strerror(errno));
	this->getRequest().setContentLength(this->getRequest().getContentLength() + written);
	this->_file_buffer.erase(0, written);
	if (this->_state == WRITE_RESPONSE && this->_file_buffer.empty()) {
		close(this->_fd);
		this->_fd = open(this->_filename.c_str(), O_RDONLY | O_NONBLOCK);
		if (this->_fd == -1) {
			this->_invalidate();
			return ;
		}
		std::cout << "Chunked body fully parsed" << std::endl;
		this->getRequest().setState(HttpRequest::BODY);
		pollfd.fd = this->_fd;
		pollfd.events = this->getEvents();
	}
}

void ChunkedBodyParser::_readFile(struct pollfd &pollfd)
{
	char buffer[FILE_READ_SIZE];

	ssize_t read_bytes = read(this->_fd, buffer, FILE_READ_SIZE);
	if (read_bytes == -1) {
		this->_invalidate();
		return ;
	}
	this->_chunk_size += read_bytes;
	this->getRequest().getBodyBuffer().append(buffer, read_bytes);
	if (this->_chunk_size == this->getRequest().getContentLength()) {
		this->getRequest().setState(HttpRequest::DONE);
		pollfd.events = 0;
	}
}

int ChunkedBodyParser::_readChunkSize() {
	size_t pos = this->_buffer.find("\r\n");
	if (pos == std::string::npos)
		return 1;
	std::string chunk_size_str = this->_buffer.substr(0, pos);
	char *endptr;
	this->_chunk_size = std::strtoul(chunk_size_str.c_str(), &endptr, 16);
	if (errno == ERANGE || *endptr != '\0') {
		this->_invalidate();
		return 1;
	}
	this->_buffer.erase(0, pos + 2);
	if (this->_chunk_size == 0) {
		this->_state = READ_CHUNK_END;
		return 0;
	}
	this->_state = READ_CHUNK;
	return 0;
}

int ChunkedBodyParser::_readChunk() {
	size_t	buffer_size = this->_buffer.size();
	this->_file_buffer.append(this->_buffer, 0, this->_chunk_size);
	this->_buffer.erase(0, this->_chunk_size);
	if (this->_chunk_size > buffer_size) {
		this->_chunk_size -= buffer_size;
		return 1;
	}
	this->_state = READ_CHUNK_END;
	return 0;
}

int ChunkedBodyParser::_readChunkEnd() {
	if (this->_buffer.size() < 2)
		return 1;
	if (this->_buffer[0] != '\r' || this->_buffer[1] != '\n') {
		this->_invalidate();
		return 1;
	}
	this->_buffer.erase(0, 2);
	if (this->_chunk_size == 0) {
		this->_state = WRITE_RESPONSE;
		std::cout << "Chunked body fully parsed" << std::endl;
		return 1;
	}
	this->_chunk_size = 0;
	this->_state = READ_CHUNK_SIZE;
	return 0;
}

void ChunkedBodyParser::_invalidate() {
	this->getRequest().setState(HttpRequest::INVALID);
	this->_state = INVALID;
}
