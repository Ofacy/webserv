/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:03:43 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/13 18:17:59 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bind.hpp"
#include "Client.hpp"
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <arpa/inet.h>

#include "StatusHttpResponse.hpp"

Client::Client(Bind &bind, int fd, struct sockaddr_in addr) : _state(READ), _bind(bind), _fd(fd), _addr(addr), _response(NULL)
{
	std::cout << "Client " << inet_ntoa(addr.sin_addr) << " created" << std::endl;
}

Client::Client(const Client &src) : _bind(src._bind), _response(NULL)
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
	delete this->_response;
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
	(void)config;
	if (pollfd.revents & POLLIN) {
		return this->_updateRead(pollfd, config);
	}
	if (pollfd.revents & POLLOUT) {
		return this->_updateWrite(pollfd, config);
		//std::cout << "Client " << this->_fd << " is ready to write" << std::endl;
	}
	else if (pollfd.revents == POLLHUP) {
		//std::cout << "Client " << this->_fd << " disconnected" << std::endl;
		return (-1);
	}
	std::cout << "Client " << this->_fd << " unknown event" << std::endl;
	return (1);
}

int	Client::_updateRead(struct pollfd &fd, Configuration &config) {
	char buffer[CLIENT_RECV_SIZE];
	(void)config;
	(void)fd;
	int ret = recv(this->_fd, buffer, CLIENT_RECV_SIZE, 0);
	if (ret < 0)
		return (-1);
	this->_request.update(buffer, ret);
	if (this->_request.isHeaderDone()) {
		// GET THE RESPONSE
		this->_response = this->_bind.getResponse(this->_request, config);
		//this->_response = new StatusHttpResponse(this->_request, 200);
		this->_state = this->_request.hasBody() ? READ_WRITE : WRITE;
		fd.events = this->getEvents();
	}
	if (this->_request.isDone())
		this->_state = WRITE;
	return (1);
}

int Client::_updateWrite(struct pollfd &fd, Configuration &config) {
	(void)config;
	(void)fd;
	return this->_response->writeResponse(this->_fd);
}