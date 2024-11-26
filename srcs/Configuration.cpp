/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:09:24 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/25 21:3651 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AccessLog.hpp"
#include "Configuration.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>

bool Configuration::_exit = false;

Configuration::Configuration(void) : InheritedParameters(), _client_max_header_size(DEFAULT_MAX_HEADER_SIZE) {
};

Configuration::Configuration(const Configuration &src) : InheritedParameters(src), _client_max_header_size(DEFAULT_MAX_HEADER_SIZE) {
	*this = src;
}

Configuration::Configuration(const std::string &config_path) : InheritedParameters(), _client_max_header_size(DEFAULT_MAX_HEADER_SIZE) {
	std::ifstream file(config_path.c_str());

	try {
		if (!file.is_open()) {
			throw std::runtime_error(std::string("Failed to open file '") + config_path + "': " + std::strerror(errno));
		}
		Lexer lexer(file);
		Parser parser(lexer);
		this->parse(parser.getRoot(), std::vector<std::string>(1, "server"));
	}
	catch (std::exception &except) {
		std::cerr << except.what() << std::endl;
		if (AccessLog::getInstance())
			delete AccessLog::getInstance();
		this->exit();
		return ;
	}
	this->addPollElement(AccessLog::getInstance());
}

Configuration::~Configuration(void) {
	for (size_t i = 0; i < this->_poll_elements.size(); i++) {
		delete this->_poll_elements[i];
	}
};

Configuration &Configuration::operator=(const Configuration &rhs) {
	if (this == &rhs)
		return (*this);
	InheritedParameters::operator=(rhs);
	this->_binds = rhs._binds;
	return (*this);
}

void	Configuration::addPollElement(IPollElement *poll_element) {
	struct pollfd pollfd;

	if (!poll_element)
		return ;
	pollfd.fd = poll_element->getFd();
	pollfd.events = poll_element->getEvents();
	pollfd.revents = 0;
	this->_poll_elements.push_back(poll_element);
	this->_pollfds.push_back(pollfd);
}

void	Configuration::removePollElement(IPollElement *poll_element)
{
	for (size_t i = 0; i < this->_poll_elements.size(); i++) {
		if (this->_poll_elements[i] == poll_element) {
			this->_poll_elements.erase(this->_poll_elements.begin() + i);
			this->_pollfds.erase(this->_pollfds.begin() + i);
			delete poll_element;
			if (i <= this->_polli)
				this->_polli--;
			break;
		}
	}
}

size_t	Configuration::getClientMaxHeaderSize() const {
	return (this->_client_max_header_size);
}

bool	Configuration::parseAttribute(const Attribute &child) {
	if (InheritedParameters::parseAttribute(child))
		return (true);
	if (child.getName() == "server") {
		this->_assignServer(child);
		return (true);
	}
	if (child.getName() == "access_log") {
		AccessLog::getInstance(child.getParameters(1)[0]);
		return (true);
	}
	if (child.getName() == "client_max_header_size") {
		char *endPtr;
		this->_client_max_header_size = std::strtoul(child.getParameters(1)[0].c_str(), &endPtr, 10);
		if (*endPtr != '\0' || errno == ERANGE)
			throw std::runtime_error("Invalid value for client_max_header_size");
		return (true);
	}
	return (false);
}

void Configuration::_assignServer(const Attribute &server_attribute) {
	Server server(server_attribute);
	Bind &bind = this->_getBind(server);
	bind.push_server(server);
}

Bind	&Configuration::_getBind(const Server &server) {
	std::string host = server.getHost();
	int port = server.getPort();
	if (host.find(':') != std::string::npos)
	{
		std::string port_str = host.substr(host.find(':') + 1);
		host = host.substr(0, host.find(':'));
		port = std::atoi(port_str.c_str());
	}
	for (size_t i = 0; i < this->_binds.size(); i++) {
		if (this->_binds[i]->isHost(host) && this->_binds[i]->getPort() == port)
			return (*this->_binds[i]);
	}
	Bind *bind = new Bind(host, port);
	this->_binds.push_back(bind);
	this->addPollElement(bind);
	return (*this->_binds.back());
}

void Configuration::start() {
	while (!Configuration::_exit) {
		if (::poll(&this->_pollfds[0], this->_pollfds.size(), -1) == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Failed to poll: " + std::string(std::strerror(errno)));
		}
		for (this->_polli = 0; this->_polli < this->_pollfds.size(); this->_polli++) {
			IPollElement *poll_element = this->_poll_elements[this->_polli];
			struct pollfd pollfd = this->_pollfds[this->_polli];
			if (poll_element->update(pollfd, *this) <= 0)
			{
				this->_poll_elements.erase(this->_poll_elements.begin() + this->_polli);
				this->_pollfds.erase(this->_pollfds.begin() + this->_polli);
				this->_polli--;
				delete poll_element;
			}
			else
				this->_pollfds[this->_polli] = pollfd;
		}
	}
}

void	Configuration::exit()
{
	Configuration::_exit = true;
}