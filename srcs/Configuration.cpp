/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:09:24 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 19:26:11 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>

bool Configuration::_exit = false;

Configuration::Configuration(void) {};

Configuration::Configuration(const Configuration &src) {
	*this = src;
}

Configuration::Configuration(const std::string &config_path) {
	std::ifstream file(config_path.c_str());

	if (!file.is_open()) {
		throw std::runtime_error(std::string("Failed to open file '") + config_path + "': " + std::strerror(errno));
	}
	Lexer lexer(file);
	Parser parser(lexer);
	std::cout << parser << std::endl;
	for (size_t i = 0; i < parser.getAttributes().size(); i++) {
		if (parser.getAttributes()[i].getName() == "server") {
			this->_assignServer(parser.getAttributes()[i]);
		}
		else
			throw std::runtime_error("Configuration file contains unknown attribute (expecting only 'server' on root)");
	}
	this->_poll();
}

Configuration::~Configuration(void) {
	for (size_t i = 0; i < this->_poll_elements.size(); i++) {
		delete this->_poll_elements[i];
	}
};

Configuration &Configuration::operator=(const Configuration &rhs) {
	if (this == &rhs)
		return (*this);
	this->_binds = rhs._binds;
	return (*this);
}

void	Configuration::addPollElement(IPollElement *poll_element) {
	struct pollfd pollfd;

	pollfd.fd = poll_element->getFd();
	pollfd.events = poll_element->getEvents();
	pollfd.revents = 0;
	this->_poll_elements.push_back(poll_element);
	this->_pollfds.push_back(pollfd);
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
		if (this->_binds[i]->getHost() == host && this->_binds[i]->getPort() == port)
			return (*this->_binds[i]);
	}
	Bind *bind = new Bind(host, port);
	this->_binds.push_back(bind);
	this->addPollElement(bind);
	return (*this->_binds.back());
}

void Configuration::_poll() {
	std::cout << "Poll elements: " << this->_poll_elements.size() << std::endl;
	std::cout << "Poll fds: " << this->_pollfds.size() << std::endl;
	while (!Configuration::_exit) {
		if (::poll(&this->_pollfds[0], this->_pollfds.size(), -1) == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Failed to poll: " + std::string(std::strerror(errno)));
		}
		for (size_t i = 0; i < this->_pollfds.size(); i++) {
			if (this->_pollfds[i].revents == 0)
				continue;
			IPollElement *poll_element = this->_poll_elements[i];
			if (poll_element->update(this->_pollfds[i], *this) <= 0)
			{
				this->_poll_elements.erase(this->_poll_elements.begin() + i);
				this->_pollfds.erase(this->_pollfds.begin() + i);
				i--;
			}
		}
	}
}

void	Configuration::exit()
{
	Configuration::_exit = true;
}