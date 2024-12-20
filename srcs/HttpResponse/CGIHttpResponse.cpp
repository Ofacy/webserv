
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHttpResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 14:54:22 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/18 15:45:18 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHttpResponse.hpp"
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>
#include <cerrno>
#include <iostream>
#include "HttpMessage.hpp"

CGIHttpResponse::CGIHttpResponse(HttpRequest &request, const std::string &path, const std::string &cgi_path) : AHttpResponse(request), _fd(-1) {
	if (access(cgi_path.c_str(), X_OK) == -1)
		throw std::runtime_error("CGI script not found or not executable: " + cgi_path);
	this->_forkCGI(path, cgi_path);
}

CGIHttpResponse::CGIHttpResponse(const CGIHttpResponse &src) : AHttpResponse(src) {
	*this = src;
}

CGIHttpResponse::~CGIHttpResponse(void) {
	if(this->_fd != -1)
		close(this->_fd);
	if (this->_pid != -1)
	{
		kill(this->_pid, SIGKILL);
		this->_waitFork();
	}
}

CGIHttpResponse &CGIHttpResponse::operator=(const CGIHttpResponse &rhs) {
	if (this == &rhs)
		return (*this);
	AHttpResponse::operator=(rhs);
	this->_cgi_headers = rhs._cgi_headers;
	this->_pid = rhs._pid;
	this->_fd = rhs._fd;
	return *this;
}

IPollElement	*CGIHttpResponse::getPollElement() {
	return this;
}

int	CGIHttpResponse::getFd() const {
	return (this->_fd);
}

short	CGIHttpResponse::getEvents() const {
	return (POLLOUT | POLLIN);
}

int	CGIHttpResponse::update(struct pollfd &pollfd, Configuration &config) {
	(void)config;
	if (pollfd.revents & POLLOUT && !(pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)))
		this->_writeCGI(pollfd);
	if (pollfd.revents & POLLIN)
		this->_readCGI(pollfd);
	if (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL)
		&& ((this->getRequest().getMethod() == "HEAD" && this->isHeaderReady()) || !(pollfd.revents & POLLIN)))
		this->_finish(pollfd);
	return 1;
}

int CGIHttpResponse::_readCGI(struct pollfd &pollfd) {
	char buffer[CGI_READ_BUFFER_SIZE];
	int ret = read(this->_fd, buffer, CGI_READ_BUFFER_SIZE);
	if (ret <= -1)
		return (-1);
	if (ret == 0)
	{
		this->_finish(pollfd);
		return 1;
	}
	if (!this->isHeaderReady())
	{
		this->_read_buffer.append(buffer, ret);
		std::string line;
		size_t pos;
		while ((pos = this->_read_buffer.find("\r\n")) != std::string::npos && !this->isHeaderReady()) {
			line = this->_read_buffer.substr(0, pos);
			if (line.empty())
				this->_finishHeader();
			else
				this->_parseHeaderLine(pollfd, line);
			this->_read_buffer.erase(0, pos + 2);
		}
		if (this->isHeaderReady())
		{
			if (this->getRequest().getMethod() != "HEAD")
				this->appendBody(this->_read_buffer);
			this->_read_buffer.clear();
		}
	}
	else if (this->getRequest().getMethod() != "HEAD")
		this->appendBody(buffer, ret);
	return 1;
}

int CGIHttpResponse::_writeCGI(struct pollfd &pollfd) {
	if (this->getRequest().getBodyBuffer().empty())
		return 1;
	int ret = send(this->_fd, this->getRequest().getBodyBuffer().c_str(), this->getRequest().getBodyBuffer().size(), MSG_NOSIGNAL);
	if (ret == -1) {
		this->_finish(pollfd);
		return 1;
	}
	this->getRequest().getBodyBuffer().erase(0, ret);
	if (this->getRequest().isDone() && this->getRequest().getBodyBuffer().empty())
		pollfd.events = POLLIN;
	return (1);
}

void	CGIHttpResponse::_finishHeader() {
	if (this->_cgi_headers.find("Status") == this->_cgi_headers.end())
		this->_cgi_headers["Status"] = "200 OK";
	this->createHeaderBuffer(atoi(this->_cgi_headers["Status"].c_str()), this->_cgi_headers);
}

void	CGIHttpResponse::_forkCGI(const std::string &script_path, const std::string &cgi_path) {
	int socket_pair[2];
	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, socket_pair) == -1)
		throw std::runtime_error("Failed to create socket pair: " + std::string(std::strerror(errno)));
	this->_pid = fork();
	if (this->_pid == -1) {
		close(socket_pair[SOCKET_PARENT]);
		close(socket_pair[SOCKET_CHILD]);
		throw std::runtime_error("Failed to fork: " + std::string(std::strerror(errno)));
	}
	else if (this->_pid == 0) {
		close(socket_pair[SOCKET_PARENT]);
		if ((this->getRequest().getContentLength() > 0 && dup2(socket_pair[SOCKET_CHILD], STDIN_FILENO) == -1)
			|| dup2(socket_pair[SOCKET_CHILD], STDOUT_FILENO) == -1)
			std::exit(1);
		else if (this->getRequest().getContentLength() == 0) {
			int	fd[2];

			if (pipe(fd) == -1)
				std::exit(1);
			if (dup2(fd[0], STDIN_FILENO) == -1)
				std::exit(1);
			close(fd[0]);
			close(fd[1]);
		}
		close(socket_pair[SOCKET_CHILD]);
		std::vector<std::string> env = this->_generateForkEnv(script_path);
		std::vector<char *> env_c;
		for (size_t i = 0; i < env.size(); i++)
			env_c.push_back(const_cast<char *>(env[i].c_str()));
		env_c.push_back(NULL);
		std::vector<char *> argv_c;
		argv_c.push_back(const_cast<char *>(cgi_path.c_str()));
		argv_c.push_back(const_cast<char *>(script_path.c_str()));
		argv_c.push_back(NULL);
		execve(cgi_path.c_str(), &(argv_c.front()), &(env_c.front()));
		std::cerr << "Failed to execve: " << std::strerror(errno) << std::endl;
		std::exit(1);
	}
	close(socket_pair[SOCKET_CHILD]);
	this->_fd = socket_pair[SOCKET_PARENT];
	if (fcntl(this->_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		this->~CGIHttpResponse();
		throw std::runtime_error("Failed to set socket to non-blocking: " + std::string(std::strerror(errno)));
	}
}

std::vector<std::string>	CGIHttpResponse::_generateForkEnv(const std::string &script_path) {
	std::vector<std::string> env;

	for (std::map<std::string, std::string>::const_iterator it = this->getRequest().getHeaders().begin(); it != this->getRequest().getHeaders().end(); it++) {
		std::string name = it->first;
		for (size_t i = 0; i < name.size(); i++)
		{
			name[i] = std::toupper(name[i]);
			if (name[i] == '-')
				name[i] = '_';
		}
		env.push_back("HTTP_" + name + "=" + it->second);
	}
	env.push_back("PATH_INFO=" + this->getRequest().getUri());
	env.push_back("SCRIPT_FILENAME=" + script_path);
	env.push_back("REQUEST_METHOD=" + this->getRequest().getMethod());
	env.push_back("SERVER_PROTOCOL=" + std::string(HTTP_VERSION));
	env.push_back("REQUEST_URI=" + this->getRequest().getUri());
	env.push_back("REDIRECT_STATUS=CGI");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_SOFTWARE=webserv");
	if (this->getRequest().getContentLength() != 0) {
		std::stringstream ss;
		ss << this->getRequest().getContentLength();
		env.push_back("CONTENT_LENGTH=" + ss.str());
	}
	if (!this->getRequest().getHeader("Content-Type").empty())
		env.push_back("CONTENT_TYPE=" + this->getRequest().getHeader("Content-Type"));
	return env;
}

void	CGIHttpResponse::_parseHeaderLine(struct pollfd &pollfd, std::string &line) {
	size_t pos = line.find(':');
	if (pos == std::string::npos) {
		this->_finish(pollfd);
		return ;
	}
	std::string key = line.substr(0, pos);
	if (key.find_first_not_of(" \t") == std::string::npos) {
		this->_finish(pollfd);
		return ;
	}
	size_t value_pos = line.find_first_not_of(" \t", pos + 1);
	if (value_pos == std::string::npos)
		value_pos = 0;
	std::string value = line.substr(value_pos);
	this->_cgi_headers[key] = value;
}

int	CGIHttpResponse::_waitFork() {
	int status;
	waitpid(this->_pid, &status, 0);
	this->_pid = -1;
	return status;
}

void	CGIHttpResponse::_finish(struct pollfd &pollfd) {
	if (this->_pid != -1) {
		kill(this->_pid, SIGKILL);
		this->_waitFork();
		this->_pid = -1;
		if (!this->isHeaderReady())
			this->createHeaderBuffer(500, std::map<std::string, std::string>());
	}
	this->setBufferDone(true);
	pollfd.events = 0;
}
