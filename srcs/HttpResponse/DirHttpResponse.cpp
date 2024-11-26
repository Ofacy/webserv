/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirHttpResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 18:52:59 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/26 13:35:59 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "DirHttpResponse.hpp"
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <iostream>

DirHttpResponse::DirHttpResponse(const DirHttpResponse &src) : AHttpResponse(src) {
	*this = src;
}

DirHttpResponse::DirHttpResponse(HttpRequest &request, uint16_t status, const std::string &path) : AHttpResponse(request) {
	std::string body(HTML_DIR);
	std::map<std::string, std::string> headers;
	DIR *dir;
	
	dir = opendir(path.c_str());
	if (dir == NULL) {
		throw std::runtime_error("Error: could not open directory: " + std::string(strerror(errno)));
	}
	
	body.replace(body.find("#title#"), 7, request.getUri());
	body.replace(body.find("#path#"), 6, request.getUri());

	std::stringstream ss;
	while (struct dirent *entry = readdir(dir)) {
		if (std::string(entry->d_name) == ".") {
			continue;
		}
		ss << this->_buildDirectoryElement(entry->d_name, path);
	}
	body.replace(body.find("#table#"), 7, ss.str());
	this->createHeaderBuffer(status, headers);
	closedir(dir);
	if (request.getMethod() != "HEAD")
		this->appendBody(body);
	this->setBufferDone(true);
}

DirHttpResponse::~DirHttpResponse(void) {}

DirHttpResponse &DirHttpResponse::operator=(const DirHttpResponse &rhs) {
	AHttpResponse::operator=(rhs);
	return *this;
}

std::string DirHttpResponse::_buildDirectoryElement(const std::string &file, const std::string &path) {
	struct stat statbuf;
	
	if (stat((path + "/" + file).c_str(), &statbuf) == -1) {
		return "";
	}
	std::stringstream ss;
	
	S_ISDIR(statbuf.st_mode) ? this->_buildDirRow(ss, file) : this->_buildFileRow(ss, file, statbuf.st_size);
	return ss.str();
}

void DirHttpResponse::_buildFileRow(std::stringstream &ss, const std::string &file, const long size)
{
	ss << "<tr><td><span class='icon'>📄</span><a href='" << _getUriPath(file) << "'>" << file << "</a></td><td>" << (size / 1024) << "KB</td></tr>";
}
void DirHttpResponse::_buildDirRow(std::stringstream &ss, const std::string &file)
{
	ss << "<tr><td class='folder' ><span class='icon'>📁</span><a href='" << _getUriPath(file) << "'>" << file << "</a></td><td>—</td></tr>";
}

std::string DirHttpResponse::_getUriPath(const std::string &file)
{
	std::string uripath = this->getRequest().getUri();
	if (uripath[uripath.size() - 1] != '/') {
		uripath += "/";
	}
	uripath += file;
	return uripath;
}
