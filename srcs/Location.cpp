/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:25:45 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 18:21:27 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <algorithm>
#include "Location.hpp"

Location::Location(void) {}

Location::Location(const Location &src) {
	*this = src;
}

Location::Location(const Attribute &root, const InheritedParameters &inherited) : InheritedParameters(inherited) {
	this->_allowed_methods = std::vector<std::string>(3);
	this->_allowed_methods[0] = "GET";
	this->_allowed_methods[1] = "POST";
	this->_allowed_methods[2] = "DELETE";
	this->_path = root.getParameters(1)[0];
	this->parse(root, std::vector<std::string>(1, "root"));
}

Location::~Location(void) {}

Location	&Location::operator=(const Location &rhs) {
	if (this == &rhs)
		return (*this);
	this->_path = rhs._path;
	this->_max_body_size = rhs._max_body_size;
	this->_root = rhs._root;
	this->_allowed_methods = rhs._allowed_methods;
	this->_autoindex = rhs._autoindex;
	this->_index = rhs._index;
	this->_cgi_paths = rhs._cgi_paths;
	return (*this);
}

const std::string	&Location::getPath(void) const {
	return (this->_path);
}

AHttpResponse * Location::getResponse(HttpRequest & request) const
{
	if (request.getUri().find("..") != std::string::npos)
		return (this->getErrorResponse(request, 400, this->_root));
	std::string path = request.getUri().substr(this->_path.size());
	if (path[0] != '/')
		path.insert(0, "/");
	return this->prepareResponse(request, this->_root, path);
}

bool	Location::parseAttribute(const Attribute &child) {
	if (InheritedParameters::parseAttribute(child))
		return (true);
	if (child.getName() == "root") {
		this->assertNotAlreadyDefined("root");
		this->_root = child.getParameters(1)[0];
		return (true);
	}
	else if (child.getName() == "limit_except") {
		this->assertNotAlreadyDefined("limit_except");
		for (std::vector<std::string>::const_iterator it = child.getParameters().begin(); it != child.getParameters().end(); it++) {
			if (std::find(this->_allowed_methods.begin(), this->_allowed_methods.end(), *it) == this->_allowed_methods.end())
				throw std::runtime_error("Invalid method " + *it + " in limit_except");
		}
		this->_allowed_methods = child.getParameters();
		return (true);
	}
	else if (child.getName() == "autoindex") {
		this->assertNotAlreadyDefined("autoindex");
		std::string value = child.getParameters(1)[0];
		if (value == "on")
			this->_autoindex = true;
		else if (value == "off")
			this->_autoindex = false;
		else
			throw std::runtime_error("Invalid value for autoindex: " + value);
		return (true);
	}
	else if (child.getName() == "index") {
		this->assertNotAlreadyDefined("index");
		this->_index = child.getParameters();
		return (true);
	}
	else if (child.getName() == "error_page") {
		std::vector<std::string> parameters = child.getParameters();
		if (parameters.size() < 2)
			throw std::runtime_error("error_page must have at least 2 parameters");
		for (size_t i = 0; i < parameters.size() - 1; i++)
			this->_error_pages[std::atoi(parameters[i].c_str())] = parameters.back();
		return (true);
	}
	else if (child.getName() == "cgi") {
		std::vector<std::string> parameters = child.getParameters();
		if (parameters.size() < 2)
			throw std::runtime_error("cgi must have at least 2 parameters");
		for (size_t i = 0; i < parameters.size() - 1; i++)
			this->_cgi_paths[parameters[i]] = parameters.back();
		return (true);
	}
	return (false);
	//	throw InvalidAttributeException(child.getName(), "server");
}

