/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:03:43 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/12 19:06:28 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>
#include <poll.h>

#include <arpa/inet.h>

Client::Client(Bind &bind, int fd, struct sockaddr_in addr) : _state(READ), _bind(bind), _fd(fd), _addr(addr)
{
	std::cout << "Client " << inet_ntoa(addr.sin_addr) << " created" << std::endl;
}

Client::Client(const Client &src) : _bind(src._bind)
{
	*this = src;
}

Client &Client::operator=(const Client &rhs)
{
	if (this == &rhs)
		return (*this);
	this->_addr = rhs._addr;
	this->_bind = rhs._bind;
	this->_fd = rhs._fd;
	return (*this);
}

Client::~Client()
{
	std::cout << "Client " << this->_fd << " destroyed" << std::endl;
	close(this->_fd);
}

int Client::getFd() const
{
	return (this->_fd);
}

short	Client::getEvents() const
{
	switch (this->_state)
	{
		case READ:
			return (POLLIN);
		case READ_WRITE:
			return (POLLIN | POLLOUT);
		case WRITE:
			return (POLLOUT);
		default:
			return (0);
	}
}

int	Client::update(struct pollfd &pollfd, Configuration &config) {
	this->_response->updateBodyBuffer(this->_request.getBodyBuffer());
	(void)config;
	if (pollfd.revents & POLLIN) {
		return this->_updateRead(pollfd, config);
	}
	if (pollfd.revents & POLLOUT) {
		//std::cout << "Client " << this->_fd << " is ready to write" << std::endl;
	}
	else if (pollfd.revents == POLLHUP) {
		//std::cout << "Client " << this->_fd << " disconnected" << std::endl;
		return (-1);
	}
	return (1);
}

int	Client::_updateRead(struct pollfd &fd, Configuration &config) {
	char buffer[CLIENT_RECV_SIZE];

	int ret = recv(this->_fd, buffer, CLIENT_RECV_SIZE, 0);
	if (ret < 0)
		return (-1);
	this->_request.update(buffer, ret);
	if (this->_request.isHeaderDone()) {
		this->_response.setRequest(this->_request);
	}
}