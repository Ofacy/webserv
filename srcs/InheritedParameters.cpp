/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InheritedParameters.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 17:31:09 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/26 13:19:37 by lcottet          ###   ########lyon.fr   */
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
#include "UploadHttpResponse.hpp"
#include "InheritedParameters.hpp"

const std::string	InheritedParameters::_supported_methods[NB_SUPPORTED_METHODS] = {
	"GET",
	"HEAD",
	"POST",
	"DELETE",
	"PUT"
};

InheritedParameters::InheritedParameters(void) {
	this->_max_body_size = 0;
	this->_allowed_methods = std::vector<std::string>(InheritedParameters::_supported_methods, InheritedParameters::_supported_methods + NB_SUPPORTED_METHODS);
	this->_autoindex = false;
	this->_index = std::vector<std::string>();
	this->_error_pages = std::map<uint16_t, std::string>();
}

InheritedParameters::InheritedParameters(const InheritedParameters &src) : AAttributeParser(src) {
	*this = src;
}

InheritedParameters::~InheritedParameters(void) {}

InheritedParameters	&InheritedParameters::operator=(const InheritedParameters &rhs) {
	if (this == &rhs)
		return (*this);
	this->_max_body_size = rhs._max_body_size;
	this->_allowed_methods = rhs._allowed_methods;
	this->_autoindex = rhs._autoindex;
	this->_index = rhs._index;
	this->_error_pages = rhs._error_pages;
	this->_cgi_paths = rhs._cgi_paths;
	this->_upload_folder = rhs._upload_folder;
	this->_return = rhs._return;
	return (*this);
}

bool	InheritedParameters::parseAttribute(const Attribute &child) {
	if (child.getName() == "client_max_body_size") {
		this->assertNotAlreadyDefined("client_max_body_size");
		char *endPtr;
		
		this->_max_body_size = std::strtoul(child.getParameters(1)[0].c_str(), &endPtr, 10);
		if (*endPtr != '\0' || errno == ERANGE)
			throw std::runtime_error("Invalid value for client_max_body_size");
		return (true);
	}
	else if (child.getName() == "limit_except") {
		this->assertNotAlreadyDefined("limit_except");
		for (std::vector<std::string>::const_iterator it = child.getParameters().begin(); it != child.getParameters().end(); it++) {
			if (std::find(
				InheritedParameters::_supported_methods,
				InheritedParameters::_supported_methods + NB_SUPPORTED_METHODS, *it)
					== InheritedParameters::_supported_methods + NB_SUPPORTED_METHODS)
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
	else if (child.getName() == "upload_folder") {
		this->assertNotAlreadyDefined("upload_folder");
		this->_upload_folder = child.getParameters(1)[0];
		return (true);
	}
	else if (child.getName() == "return") {
		std::vector<std::string> parameters = child.getParameters();
		if (parameters.size() < 1)
			throw std::runtime_error("return must have at least 1 parameters");
		char *endPtr;
		uint16_t status_code = std::strtol(parameters[0].c_str(), &endPtr, 10);
		if (*endPtr != '\0' || status_code < 200 || status_code > 599)
			throw std::runtime_error("Invalid status code in return");
		if (parameters.size() < 2)
			this->_return = std::make_pair(status_code, "");
		else
		{
			std::stringstream ss;
			for (size_t i = 1; i < parameters.size(); i++)
			{
				ss << parameters[i] << (i == parameters.size() - 1 ? "" : " ");
			}
			this->_return = std::make_pair(status_code, ss.str());
		}
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

const std::pair<uint16_t, std::string>	&InheritedParameters::getReturn(void) const {
	return (this->_return);
}

AHttpResponse *InheritedParameters::prepareResponse(HttpRequest &request, const std::string &root, const std::string &uri) const {
	if (this->_return.first != 0) {
		if (this->_return.second.empty())
			return (this->getErrorResponse(request, this->_return.first, root));
		return (new StatusHttpResponse(request, this->_return.first, this->_return.second));
	}
	if (std::find(this->_allowed_methods.begin(), this->_allowed_methods.end(), request.getMethod()) == this->_allowed_methods.end())
		return (this->getErrorResponse(request, 405, root));
	if (request.getContentLength() > this->_max_body_size && this->_max_body_size != 0)
		return (this->getErrorResponse(request, 413, root));
	request.setMaxBodySize(this->_max_body_size);
	std::string path = root + uri;
	if (request.getMethod() == "PUT") {
		AHttpResponse *response = this->_getCGIResponse(request, path);
		if (response)
			return (response);
		path = this->_upload_folder + uri;
		if (this->_upload_folder.empty())
			path = root + uri;
		return new UploadHttpResponse(request, path);
	}
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == -1)
		return (this->getErrorResponse(request, 404, root));
	if (S_ISDIR(buffer.st_mode)) {
		return (this->_getDirectoryResponse(request, path, root));
	}
	AHttpResponse *response = this->_getCGIResponse(request, path);
	if (response)
		return (response);
	if (request.getMethod() == "DELETE") {
		path = this->_upload_folder + uri;
		if (this->_upload_folder.empty())
			path = root + uri;
		if (std::remove(path.c_str()) == -1)
			return (this->getErrorResponse(request, 404, root));
		return (new StatusHttpResponse(request, 200));
	}
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return (this->getErrorResponse(request, 404, root));
	return (new FileHttpResponse(request, 200, fd, buffer));
}

AHttpResponse *InheritedParameters::getErrorResponse(HttpRequest &request, const uint16_t status_code, const std::string & root) const
{
	std::map<uint16_t, std::string>::const_iterator it = this->_error_pages.find(status_code);
	if (it == this->_error_pages.end())
		return (new StatusHttpResponse(request, status_code));
	struct stat buffer;
	std::string path = root + it->second;
	if (stat(path.c_str(), &buffer) == -1)
		return (new StatusHttpResponse(request, status_code));
	if (S_ISDIR(buffer.st_mode))
		return (this->_getDirectoryResponse(request, path, root));
	AHttpResponse *response = this->_getCGIResponse(request, path);
	if (response)
		return (response);
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
			AHttpResponse *response = this->_getCGIResponse(request, index_path);
			if (response)
				return (response);
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

AHttpResponse	*InheritedParameters::_getCGIResponse(HttpRequest &request, const std::string &path) const {
	std::map<std::string, std::string>::const_iterator it = this->_cgi_paths.begin();
	for (; it != this->_cgi_paths.end(); it++) {
		if (path.size() > it->first.size() && path.find(it->first, path.size() - it->first.size()) != std::string::npos)
			break ;
	}
	if (it != this->_cgi_paths.end())
		return (new CGIHttpResponse(request, path, it->second));
	return (NULL);
}
