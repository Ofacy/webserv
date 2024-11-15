/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:25:45 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/15 17:30:23 by bwisniew         ###   ########.fr       */
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
	this->_path = root.getParameters(1)[0];
	this->parse(root, std::vector<std::string>(1, "root"));
}

Location::~Location(void) {}

Location	&Location::operator=(const Location &rhs) {
	if (this == &rhs)
		return (*this);
	InheritedParameters::operator=(rhs);
	this->_path = rhs._path;
	this->_root = rhs._root;
	return (*this);
}

const std::string	&Location::getPath(void) const {
	return (this->_path);
}

AHttpResponse * Location::getResponse(HttpRequest & request) const
{
	std::cout << "Location: " << this->_path << std::endl;
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
	return (false);
	//	throw InvalidAttributeException(child.getName(), "server");
}

