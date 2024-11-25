/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:03:43 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/25 17:23:25 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Bind.hpp"
#include "Client.hpp"
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include "AccessLog.hpp"

#include "StatusHttpResponse.hpp"

Client::Client(Bind &bind, int fd, struct sockaddr_in addr, Configuration &config) : _config(config), _state(READ), _bind(bind), _fd(fd), _addr(addr), _request(config), _response(NULL)
{
	//std::cout << "Client " << fd << " created" << std::endl;
}

Client::Client(const Client &src) : _config(src._config), _bind(src._bind), _request(src._config), _response(NULL)
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

std::string Client::getIp() const {
	unsigned char *byte_ip = (unsigned char *)&this->_addr.sin_addr.s_addr;
	std::stringstream ip;
	ip << (int)byte_ip[0] << "." << (int)byte_ip[1] << "." << (int)byte_ip[2] << "." << (int)byte_ip[3];
	return (ip.str());
}

Client::State	Client::getState() const {
	return (this->_state);
}

const HttpRequest	&Client::getRequest() const {
	return (this->_request);
}

const AHttpResponse	*Client::getResponse() const {
	return (this->_response);
}


int	Client::update(struct pollfd &pollfd, Configuration &config) {
	if (pollfd.revents & POLLIN && (this->_state == READ || this->_state == READ_WRITE)) {
		if (this->_updateRead() <= 0)
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
	if (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
		//std::cout << "Client " << this->_fd << " disconnected" << std::endl;
		this->_cleanResponse(config);
		return (-1);
	}
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
	}
	if (this->_request.isDone())
		this->_state = WRITE;
	pollfd.events = this->getEvents();
	return (1);
}

int	Client::_updateRead() {
	char buffer[CLIENT_RECV_SIZE];
	int ret = recv(this->_fd, buffer, CLIENT_RECV_SIZE, 0);
	if (ret < 0)
		return (-1);
	this->_request.update(buffer, ret);
	return (1);
}

int Client::_updateWrite(struct pollfd &fd, Configuration &config) {
	(void)config;
	(void)fd;
	return (this->_response->writeResponse(this->_fd));
}

void Client::_cleanResponse(Configuration &config)
{
	if (AccessLog::getInstance())
		*AccessLog::getInstance() << *this;
	if (this->_response)
	{
		if (this->_response->getPollElement() != NULL)
			config.removePollElement(this->_response->getPollElement());
		else
			delete this->_response;
		this->_response = NULL;
	}
}