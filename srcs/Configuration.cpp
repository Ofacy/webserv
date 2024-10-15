/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:09:24 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/15 18:01:08 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>

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
}

Configuration::~Configuration(void) {};

Configuration &Configuration::operator=(const Configuration &rhs) {
	if (this == &rhs)
		return (*this);
	this->_binds = rhs._binds;
	return (*this);
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
		if (this->_binds[i].getHost() == host && this->_binds[i].getPort() == port)
			return (this->_binds[i]);
	}
	this->_binds.push_back(Bind(host, port));
	return (this->_binds.back());
}
