/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:20:50 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/15 18:11:32 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include "Server.hpp"

Server::Server(void) {}

Server::Server(const Server &src) {
	*this = src;
}

Server::Server(const Attribute &root) {
	std::vector<std::string> mandatory_childs(3);
	mandatory_childs[0] = "server_name";
	mandatory_childs[1] = "location";
	mandatory_childs[2] = "listen";
	this->parse(root, mandatory_childs);
}

Server::~Server(void) {}

Server &Server::operator=(const Server &rhs) {
	if (this == &rhs)
		return (*this);
	this->_locations = rhs._locations;
	this->_server_names = rhs._server_names;
	return (*this);
}

const std::string Server::getHost() const {
	return (this->_host);
}

int	Server::getPort() const {
	return (this->_port);
}

void	Server::parseAttribute(const Attribute &child) {
	if (child.getName() == "server_name") {
		this->assertNotAlreadyDefined("server_name");
		this->_server_names = child.getParameters();
	}
	else if (child.getName() == "location") {
		Location location(child);
		if (this->_locations.find(location.getPath()) != this->_locations.end())
			throw std::runtime_error("Location " + location.getPath() + " already defined in server");
		this->_locations[location.getPath()] = location;
	}
	else if (child.getName() == "listen") {
		this->assertNotAlreadyDefined("listen");
		std::vector<std::string> parameters = child.getParameters(2);
		this->_host = parameters[0];
		this->_port = std::atoi(parameters[1].c_str());
	}
	//else
	//	throw InvalidAttributeException(child.getName(), "server");
}
