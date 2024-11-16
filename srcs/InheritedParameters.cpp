/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InheritedParameters.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 17:31:09 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/16 17:29:02 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include "StatusHttpResponse.hpp"
#include "FileHttpResponse.hpp"
#include "DirHttpResponse.hpp"
#include "CGIHttpResponse.hpp"
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
		//std::cout << "Index size: " << this->_index.size() << std::endl;
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
		return (this->_getDirectoryResponse(request, path, root));
	}
	else {
		std::map<std::string, std::string>::const_iterator it = this->_cgi_paths.begin();
		for (; it != this->_cgi_paths.end(); it++) {
			if (uri.size() > it->first.size() && uri.find(it->first, uri.size() - it->first.size()) != std::string::npos)
				break ;
		}
		if (it != this->_cgi_paths.end()) {
			std::cout << "Found CGI path " << it->second << std::endl;
			return (new CGIHttpResponse(request, root + uri, it->second));
		}
		int fd = open(path.c_str(), O_RDONLY);
		if (fd == -1)
			return (this->getErrorResponse(request, 404, root));
		return (new FileHttpResponse(request, 200, fd, buffer));
	}
	return (NULL);
}

AHttpResponse *InheritedParameters::getErrorResponse(HttpRequest &request, const uint16_t status_code, const std::string & root) const
{
	std::map<uint16_t, std::string>::const_iterator it = this->_error_pages.find(status_code);
	if (it == this->_error_pages.end())
		return (new StatusHttpResponse(request, status_code));
	struct stat buffer;
	std::string path = root + it->second;

	//std::cout << "Error page path: " << path << std::endl;
	if (stat(path.c_str(), &buffer) == -1)
		return (new StatusHttpResponse(request, status_code));
	if (S_ISDIR(buffer.st_mode))
		return (this->_getDirectoryResponse(request, path, root));
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return (new StatusHttpResponse(request, status_code));
	return (new FileHttpResponse(request, status_code, fd, buffer));
}

AHttpResponse *InheritedParameters::getErrorResponse(HttpRequest &request, const uint16_t status_code) const
{
	return InheritedParameters::getErrorResponse(request, status_code, "");
}

AHttpResponse *InheritedParameters::_getDirectoryResponse(HttpRequest &request, const std::string &path, const std::string &root) const {
	(void)path;
	for (std::vector<std::string>::const_iterator it = this->_index.begin(); it != this->_index.end(); it++) {
		std::string index_path = path + "/" + *it;
		struct stat buffer;
		if (stat(index_path.c_str(), &buffer) == -1)
			continue ;
		if (S_ISREG(buffer.st_mode)) {
			int fd = open(index_path.c_str(), O_RDONLY);
			if (fd == -1)
				continue ;
			return (new FileHttpResponse(request, 200, fd, buffer));
		}
	}
	if (this->_autoindex)
		return (new DirHttpResponse(request, 200, path));
	return (this->getErrorResponse(request, 404, root));
}
