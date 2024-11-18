/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:03:43 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/18 19:56:42 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Bind.hpp"
#include "Client.hpp"
#include <unistd.h>
#include <poll.h>
#include <iostream>

#include "StatusHttpResponse.hpp"

Client::Client(Bind &bind, int fd, struct sockaddr_in addr) : _state(READ), _bind(bind), _fd(fd), _addr(addr), _response(NULL)
{
	std::cout << "Client " << fd << " created" << std::endl;
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
	if (this->_response && this->_response->getPollElement() == NULL)
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
	if (pollfd.revents & POLLIN) {
		if (this->_updateRead(pollfd, config) <= 0)
		{
			this->_cleanResponse(config);
			return (-1);
		}
	}
	if (pollfd.revents & POLLOUT) {
		if (this->_updateWrite(pollfd, config) <= 0)
		{
			this->_cleanResponse(config);
			return (-1);
		}
		//std::cout << "Client " << this->_fd << " is ready to write" << std::endl;
	}
	if (pollfd.revents & POLLHUP || pollfd.revents & POLLERR) {
		//std::cout << "Client " << this->_fd << " disconnected" << std::endl;
		this->_cleanResponse(config);
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
	if (this->_request.isHeaderDone() && this->_response == NULL) {
		try {
			if (this->_request.getState() == HttpRequest::INVALID)
				this->_response = new StatusHttpResponse(this->_request, 400);
			else
				this->_response = this->_bind.getResponse(this->_request, config);
		}
		catch(std::exception &except)
		{
			this->_response = new StatusHttpResponse(this->_request, 500, except.what());
		}
		if (this->_response->getPollElement() != NULL)
			config.addPollElement(this->_response->getPollElement());
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
	return (this->_response->writeResponse(this->_fd));
}

void Client::_cleanResponse(Configuration &config)
{
	if (this->_response)
	{
		if (this->_response->getPollElement() != NULL)
			config.removePollElement(this->_response->getPollElement());
		else
			delete this->_response;
		this->_response = NULL;
	}
}