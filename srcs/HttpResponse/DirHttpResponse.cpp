/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirHttpResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 18:52:59 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/15 19:18:54 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirHttpResponse.hpp"
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

DirHttpResponse::DirHttpResponse(const DirHttpResponse &src) : AHttpResponse(src) {
	*this = src;
}

DirHttpResponse::DirHttpResponse(HttpRequest &request, uint16_t status, const std::string &path) : AHttpResponse(request) {
	std::stringstream ss;
	std::map<std::string, std::string> headers;
	DIR *dir;
	
	dir = opendir(path.c_str());
	if (dir == NULL) {
		throw std::runtime_error("Error: could not open directory");
	}
	ss << HTML_DIR_BEGIN;
	while (struct dirent *entry = readdir(dir)) {
		if (std::string(entry->d_name) == ".") {
			continue;
		}
		ss << this->_buildDirectoryElement(entry->d_name, path);
	}
	ss << HTML_DIR_END;
	this->createHeaderBuffer(status, headers);
	std::string str = ss.str();
	this->appendBody(str);
	closedir(dir);
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
		throw std::runtime_error("Error: could not get file stats");
	}
	std::stringstream ss;
	std::string uripath = this->getRequest().getUri();
	if (uripath[uripath.size() - 1] != '/') {
		uripath += "/";
	}
	uripath += file;
	ss << "<tr><td><a href=\"" << uripath << "\">" << file << "</a></td><td>" << statbuf.st_size << "</td></tr></br>";
	return ss.str();
}