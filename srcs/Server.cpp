/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:20:50 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/26 17:59:26 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include "Server.hpp"

Server::Server(void) {}

Server::Server(const Server &src) : InheritedParameters(src) {
	*this = src;
}

Server::Server(const Attribute &root) : InheritedParameters() {
	std::vector<std::string> mandatory_childs(2);
	mandatory_childs[0] = "location";
	mandatory_childs[1] = "listen";
	if (root.getParameters().size() != 0)
		throw std::runtime_error("Server should not have parameters");
	this->parse(root, mandatory_childs);
	if (this->_locations.find("/") == this->_locations.end())
		throw std::runtime_error("No root location defined in server");
}

Server::~Server(void) {}

Server &Server::operator=(const Server &rhs) {
	if (this == &rhs)
		return (*this);
	InheritedParameters::operator=(rhs);
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

bool Server::hasName() const {
	return (!_server_names.empty());
}

bool Server::hasName(const std::string &name) const
{
	for (std::vector<std::string>::const_iterator it = _server_names.begin(); it != _server_names.end(); it++)
	{
		if (*it == name)
			return (true);
	}
	return (false);
}

const std::vector<std::string> &Server::getServerNames() const {
	return (_server_names);
}

AHttpResponse *Server::getResponse(HttpRequest & request) const
{
	std::map<std::string, Location>::const_iterator it = this->_locations.begin();
	size_t longest_path = 0;
	std::map<std::string, Location>::const_iterator longest_it = this->_locations.end();
	for (; it != this->_locations.end(); it++) {
		if (request.getUri().find(it->first) == 0 && it->first.size() > longest_path) {
			longest_path = it->first.size();
			longest_it = it;
		}
	}
	if (longest_it == this->_locations.end())
		return (this->getErrorResponse(request, 500)); // There should always be a location for "/"
	return (longest_it->second.getResponse(request));
}

bool	Server::parseAttribute(const Attribute &child) {
	if (InheritedParameters::parseAttribute(child))
		return (true);
	else if (child.getName() == "server_name") {
		this->assertNotAlreadyDefined("server_name");
		this->_server_names = child.getParameters();
		return (true);
	}
	else if (child.getName() == "location") {
		Location location(child, *this);
		if (this->_locations.find(location.getPath()) != this->_locations.end())
			throw std::runtime_error("Location " + location.getPath() + " already defined in server");
		this->_locations[location.getPath()] = location;
		return (true);
	}
	else if (child.getName() == "listen") {
		this->assertNotAlreadyDefined("listen");
		std::vector<std::string> parameters = child.getParameters(2);
		this->_host = parameters[0];
		this->_port = std::atoi(parameters[1].c_str());
		return (true);
	}
	return (false);
}
