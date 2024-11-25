/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AccessLog.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 16:31:16 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/25 17:23:25 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <ctime>
#include "Client.hpp"
#include "AccessLog.hpp"

AccessLog *AccessLog::_instance = NULL;

AccessLog::AccessLog(const std::string &path) : _fd(open(path.c_str(), O_CREAT | O_WRONLY | O_APPEND | O_NONBLOCK, S_IRUSR | S_IWUSR)) {
	if (_fd == -1)
		throw std::runtime_error("Failed to open access log file");
}

AccessLog::~AccessLog() {
	if (_fd != -1)
		close(_fd);
}

AccessLog *&AccessLog::getInstance(const std::string &path) {
	if (_instance)
		return _instance;
	try {
		_instance = new AccessLog(path);
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return _instance;
}

AccessLog *&AccessLog::getInstance() {
	return _instance;
}

int AccessLog::getFd() const {
	return _fd;
}

short AccessLog::getEvents() const {
	return POLLOUT;
}

int	AccessLog::update(struct pollfd &pollfd, Configuration &config) {
	(void)config;
	if (pollfd.revents & POLLOUT) {
		if (_buffer.empty())
			return 1;
		ssize_t ret = write(_fd, _buffer.c_str(), _buffer.size());
		if (ret == -1) {
			std::cerr << "Failed to write to access log file" << std::endl;
			close(_fd);
			_fd = -1;
			pollfd.events = 0;
			return -1;
		}
		_buffer.erase(0, ret);
	}
	return 1;
}

AccessLog &AccessLog::operator<<(const std::string &str) {
	_buffer += str;
	return *this;
}

AccessLog &AccessLog::operator<<(const Client &client) {
	std::stringstream ss;

	ss << client.getIp() << " - - [" << this->_getTime() << "] \"" << client.getRequest().getMethod() << " " << client.getRequest().getUri() << "\" "
		<< this->_getResponseStr(client) << " "
		<< "\"" << client.getRequest().getHeader("Host") << "\" "
		<< "\"" << client.getRequest().getHeader("User-Agent") << "\"" << std::endl;
	return *this << ss.str();
}


std::string		AccessLog::_getTime() const {
	std::time_t time = std::time(NULL);
	std::string str = std::asctime(std::localtime(&time));
    return str.substr(0, str.size() - 1);
}

std::string		AccessLog::_getResponseStr(const Client &client) const {
	std::stringstream ss;

	if (client.getResponse() == NULL)
		ss << "500";
	else
		ss << client.getResponse()->getStatus() << " " << client.getResponse()->getContentLength();
	return ss.str();
}
