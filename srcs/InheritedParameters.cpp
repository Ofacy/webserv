/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InheritedParameters.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 17:31:09 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/13 18:26:47 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include "StatusHttpResponse.hpp"
#include "InheritedParameters.hpp"

InheritedParameters::InheritedParameters(void) {
	this->_max_body_size = 0;
	this->_allowed_methods = std::vector<std::string>(3);
	this->_allowed_methods[0] = "GET";
	this->_allowed_methods[1] = "POST";
	this->_allowed_methods[2] = "DELETE";
	this->_autoindex = false;
	this->_index = std::vector<std::string>();
	this->_error_pages = std::map<uint16_t, std::string>();
}

InheritedParameters::InheritedParameters(const InheritedParameters &src) : AAttributeParser(src) {
	this->_allowed_methods = std::vector<std::string>(3);
	this->_allowed_methods[0] = "GET";
	this->_allowed_methods[1] = "POST";
	this->_allowed_methods[2] = "DELETE";
	*this = src;
}

InheritedParameters::~InheritedParameters(void) {}

InheritedParameters	&InheritedParameters::operator=(const InheritedParameters &rhs) {
	if (this == &rhs)
		return (*this);
	this->_max_body_size = rhs._max_body_size;
	//this->_allowed_methods = rhs._allowed_methods;
	this->_autoindex = rhs._autoindex;
	this->_index = rhs._index;
	this->_error_pages = rhs._error_pages;
	this->_cgi_paths = rhs._cgi_paths;
	return (*this);
}

bool	InheritedParameters::parseAttribute(const Attribute &child) {
	if (child.getName() == "client_max_body_size") {
		this->assertNotAlreadyDefined("client_max_body_size");
		this->_max_body_size = std::atoi(child.getParameters(1)[0].c_str());
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
		{
			char *endPtr;
			uint16_t status_code = std::strtol(parameters[i].c_str(), &endPtr, 10);
			if (*endPtr != '\0' || status_code < 100 || status_code > 599)
				throw std::runtime_error("Invalid status code in error_page");
			this->_error_pages[status_code] = parameters.back();
		}
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
}


size_t	InheritedParameters::getMaxBodySize(void) const {
	return (this->_max_body_size);
}

const std::vector<std::string>	&InheritedParameters::getAllowedMethods(void) const {
	return (this->_allowed_methods);
}

bool	InheritedParameters::getAutoindex(void) const {
	return (this->_autoindex);
}

const std::vector<std::string>	&InheritedParameters::getIndex(void) const {
	return (this->_index);
}

const std::map<uint16_t, std::string>	&InheritedParameters::getErrorPages(void) const {
	return (this->_error_pages);
}

const std::map<std::string, std::string>	&InheritedParameters::getCgiPaths(void) const {
	return (this->_cgi_paths);
}

AHttpResponse *InheritedParameters::prepareResponse(HttpRequest &request, const std::string &root, const std::string &uri) const {
	if (std::find(this->_allowed_methods.begin(), this->_allowed_methods.end(), request.getMethod()) == this->_allowed_methods.end())
		return (this->getErrorResponse(request, 405, root));
	std::string path = root + uri;
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == -1)
		return (this->getErrorResponse(request, 404, root));
	if (S_ISDIR(buffer.st_mode)) {
		return (this->_getDirectoryResponse(request, path));
	}
	else {
		int fd = open(path.c_str(), O_RDONLY);
		if (fd == -1)
			return (this->getErrorResponse(request, 404, root));
		return (new FileHttpResponse(200, fd, buffer));
	}
	return (NULL);
}

AHttpResponse *InheritedParameters::getErrorResponse(HttpRequest &request, const uint16_t status_code, const std::string & root) const
{
	(void)root;
	return new StatusHttpResponse(request, status_code);
}

AHttpResponse *InheritedParameters::getErrorResponse(HttpRequest &request, const uint16_t status_code) const
{
	return InheritedParameters::getErrorResponse(request, status_code, "");
}
