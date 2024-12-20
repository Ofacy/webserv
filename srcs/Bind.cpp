/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bind.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 17:36:19 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/26 17:52:46 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "StatusHttpResponse.hpp"
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
	if (this->_fd == -1)
		throw std::runtime_error("Failed to create socket: " + std::string(std::strerror(errno)));
	int opt = 1;
	if (setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
		this->~Bind();
		throw std::runtime_error("Failed to set socket options: " + std::string(std::strerror(errno)));
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = _getHost(this->_host);
	if (bind(this->_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		this->~Bind();
		throw std::runtime_error("Failed to bind socket: " + std::string(std::strerror(errno)));
	}
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

bool Bind::isHost(const std::string &host) const {
	return (this->_getHost(this->_host) == this->_getHost(host));
}

int Bind::getPort(void) const {
	return (_port);
}

void Bind::listen(void) {
	if (::listen(this->_fd, SOMAXCONN) == -1)
		throw std::runtime_error("Failed to listen on socket: " + std::string(std::strerror(errno)));
	std::cout << "Listening on " << this->_host << ":" << this->_port << std::endl;
}

Client *Bind::accept(Configuration &config) {
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	int fd = ::accept(this->_fd, (struct sockaddr *)&addr, &addr_len);
	if (fd == -1)
		return (NULL);
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		close(fd);
		throw std::runtime_error("Failed to set socket to non-blocking: " + std::string(std::strerror(errno)));
	}
	return (new Client(*this, fd, addr, config));
}

void Bind::push_server(const Server &server) {
	if (server.hasName()) {
		for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++) {
			if (it->hasName(server.getServerNames()[0]))
				throw std::runtime_error("Server with name " + server.getServerNames()[0] + " already exists for this host and port");
		}
	}
	else {
		for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++) {
			if (!server.hasName())
				throw std::runtime_error("Server without name already exists for this host and port");
		}
	}
	_servers.push_back(server);
}

uint32_t Bind::_getHost(const std::string &host_str) const {
	uint8_t	count = 0;
	char	*endPtr;
	
	if (this->_host == "")
		return INADDR_ANY;
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
	return (htonl(host));
}

int	Bind::getFd() const {
	return (this->_fd);
}

short Bind::getEvents() const {
	return (POLLIN);
}

int	Bind::update(struct pollfd &pollfd, Configuration &config) {
	if ((pollfd.revents & POLLHUP) == POLLHUP) {
		this->listen();
	}
	if ((pollfd.revents & POLLIN) == POLLIN) {
		Client *client = this->accept(config);
		if (client)
			config.addPollElement(client);
	}
	return (1);
}

AHttpResponse *Bind::getResponse(HttpRequest & request, Configuration &config)
{
	for (std::vector<Server>::reverse_iterator it = this->_servers.rbegin(); it != this->_servers.rend(); it++) {
		if (it->hasName(request.getHeader("Host")))
			return (it->getResponse(request));
	}
	for (std::vector<Server>::reverse_iterator it = this->_servers.rbegin(); it != this->_servers.rend(); it++) {
		if (!it->hasName())
			return (it->getResponse(request));
	}
	return (config.getErrorResponse(request, 404));
}
