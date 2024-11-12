/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bind.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 17:36:19 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 16:16:05 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include "Bind.hpp"

Bind::Bind(void) : _port(0), _host(""), _fd(-1) {}

Bind::Bind(const Bind &src) {
	*this = src;
}

Bind::Bind(const std::string &host, int port) : _port(port), _host(host), _fd(-1) {
	struct sockaddr_in addr;
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "bind fd: " << this->_fd << std::endl;
	if (this->_fd == -1)
		throw std::runtime_error("Failed to create socket: " + std::string(std::strerror(errno)));
	int opt = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Failed to set socket options: " + std::string(std::strerror(errno)));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = _getHost();
	if (bind(this->_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("Failed to bind socket: " + std::string(std::strerror(errno)));
}

Bind::~Bind(void) {
	if (this->_fd != -1)
		close(this->_fd);
}

Bind &Bind::operator=(const Bind &rhs) {
	if (this == &rhs)
		return (*this);
	_port = rhs._port;
	_host = rhs._host;
	_fd = rhs._fd;
	return (*this);
}

const std::string &Bind::getHost(void) const {
	return (_host);
}

int Bind::getPort(void) const {
	return (_port);
}

void Bind::listen(void) {
	if (::listen(this->_fd, SOMAXCONN) == -1)
		throw std::runtime_error("Failed to listen on socket: " + std::string(std::strerror(errno)));
	std::cout << "Listening on " << this->_host << ":" << this->_port << std::endl;
}

Client *Bind::accept(void) {
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	int fd = ::accept(this->_fd, (struct sockaddr *)&addr, &addr_len);
	if (fd == -1)
		throw std::runtime_error("Failed to accept connection: " + std::string(std::strerror(errno)));
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Failed to set socket to non-blocking: " + std::string(std::strerror(errno)));
	return (new Client(*this, fd, addr));
}

void Bind::push_server(const Server &server) {
	_servers.push_back(server);
}

uint32_t Bind::_getHost(void) const {
	uint8_t	count = 0;
	char	*endPtr;
	
	if (this->_host == "")
		return INADDR_ANY;
	std::string host_str = this->_host;
	uint32_t host = std::strtoul(host_str.c_str(), &endPtr, 10);
	if (*endPtr != '.' || host > 255 || errno == ERANGE)
		throw std::runtime_error("Invalid host: " + this->_host);
	for (size_t i = 0; i < host_str.size(); i++) {
		if (host_str[i] == '.')
		{
			unsigned long part = std::strtoul(host_str.c_str() + i + 1, &endPtr, 10);
			if ((count == 2 && *endPtr != '\0') || (count < 2 && *endPtr != '.') || part > 255 || errno == ERANGE || count > 3)
				throw std::runtime_error("Invalid host: " + this->_host);
			host = (host << 8) + part;
			count++;
		}
	}
	std::cout << "host: " << htonl(host) << std::endl;
	return (htonl(host));
}


int	Bind::getFd() const {
	return (this->_fd);
}

short Bind::getEvents() const {
	return (POLLIN);
}

int	Bind::update(struct pollfd &pollfd, Configuration &config) {
	if ((pollfd.revents & POLLHUP) == POLLHUP)
	{
		std::cout << "Bind update " << pollfd.revents << std::endl;
		this->listen();
	}
	if ((pollfd.revents & POLLIN) == POLLIN) {
		Client *client = this->accept();
		config.addPollElement(client);
	}
	return (1);
}