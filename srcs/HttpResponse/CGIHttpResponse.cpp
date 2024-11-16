/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHttpResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 14:54:22 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/16 17:24:07 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHttpResponse.hpp"
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>
#include <cerrno>
#include <iostream>
#include "HttpMessage.hpp"

CGIHttpResponse::CGIHttpResponse(HttpRequest &request, const std::string &path, const std::string &cgi_path) : AHttpResponse(request) {
	this->_forkCGI(path, cgi_path);
}

CGIHttpResponse::CGIHttpResponse(const CGIHttpResponse &src) : AHttpResponse(src) {
	*this = src;
}

CGIHttpResponse::~CGIHttpResponse(void) {
	if (this->_stdin[PIPE_READ] != -1)
		close(this->_stdin[PIPE_READ]);
	if (this->_stdin[PIPE_WRITE] != -1)
		close(this->_stdin[PIPE_WRITE]);
	if (this->_stdout[PIPE_READ] != -1)
		close(this->_stdout[PIPE_READ]);
	if (this->_stdout[PIPE_WRITE] != -1)
		close(this->_stdout[PIPE_WRITE]);
}

CGIHttpResponse &CGIHttpResponse::operator=(const CGIHttpResponse &rhs) {
	if (this == &rhs)
		return (*this);
	AHttpResponse::operator=(rhs);
	this->_cgi_headers = rhs._cgi_headers;
	this->_state = rhs._state;
	this->_pid = rhs._pid;
	this->_stdin[PIPE_READ] = rhs._stdin[PIPE_READ];
	this->_stdin[PIPE_WRITE] = rhs._stdin[PIPE_WRITE];
	this->_stdout[PIPE_READ] = rhs._stdout[PIPE_READ];
	this->_stdout[PIPE_WRITE] = rhs._stdout[PIPE_WRITE];
	return *this;
}

IPollElement	*CGIHttpResponse::getPollElement() {
	return this;
}

int	CGIHttpResponse::getFd() const {
	switch (this->_state)
	{
	case WRITE:
		return this->_stdin[PIPE_WRITE];
		break;
	default:
		return this->_stdout[PIPE_READ];
		break;
	}
}

short	CGIHttpResponse::getEvents() const {
	switch (this->_state)
	{
	case WRITE:
		return POLLOUT;
		break;
	default:
		return POLLIN;
		break;
	}
}

int	CGIHttpResponse::update(struct pollfd &pollfd, Configuration &config) {
	(void)config;
	if (this->_state == WRITE && pollfd.revents & POLLOUT)
	{
		if (this->_writeCGI(pollfd) <= 0)
			return -1;
	}
	else if (this->_state == READ && pollfd.revents & POLLIN)
	{
		if (this->_readCGI() <= 0)
			return -1;
	}
	if (pollfd.revents & (POLLERR | POLLHUP | POLLNVAL))
	{
		//std::cout << "Error on fd " << pollfd.fd << std::endl;
		this->_waitFork();
		this->setBufferDone(true);
		
		return 1;
	}
	return 1;
}

int CGIHttpResponse::_readCGI() {
	char buffer[CGI_READ_BUFFER_SIZE];
	int ret = read(this->_stdout[PIPE_READ], buffer, CGI_READ_BUFFER_SIZE);
	if (ret == -1)
		return -1;
	if (ret == 0)
	{
		this->setBufferDone(true);
		this->_waitFork();
		return (0);
	}
	if (!this->isHeaderReady())
	{
		this->_read_buffer.append(buffer, ret);
		std::string line;
		size_t pos;
		while ((pos = this->_read_buffer.find("\r\n")) != std::string::npos) {
			line = this->_read_buffer.substr(0, pos);
			if (line.empty())
			{
				this->_finishHeader();
			}
			else
				this->_parseHeaderLine(line);
			this->_read_buffer.erase(0, pos + 2);
		}
		if (this->isHeaderReady())
		{
			this->appendBody(this->_read_buffer);
		}
	}
	else
		this->appendBody(buffer, ret);
	return 1;
}

int CGIHttpResponse::_writeCGI(struct pollfd &pollfd) {
	int ret = write(this->_stdin[PIPE_WRITE], this->getRequest().getBodyBuffer().c_str(), this->getRequest().getBodyBuffer().size());
	if (ret == -1)
		return -1;
	if (ret != 0)
		std::cout << "Request body sent: " << this->getRequest().getBodyBuffer().substr(0, ret) << std::endl;
	this->getRequest().getBodyBuffer().erase(0, ret);
	if (this->getRequest().isDone())
	{
		close(this->_stdin[PIPE_WRITE]);
		this->_stdin[PIPE_WRITE] = -1;
		this->_state = READ;
		pollfd.events = this->getEvents();
		pollfd.fd = this->getFd();
	}
	return (1);
}

void	CGIHttpResponse::_finishHeader() {
	this->createHeaderBuffer(atoi(this->_cgi_headers["Status"].c_str()), this->_cgi_headers);
}

void	CGIHttpResponse::_forkCGI(const std::string &script_path, const std::string &cgi_path) {
	memset(this->_stdin, -1, sizeof(this->_stdin));
	memset(this->_stdout, -1, sizeof(this->_stdout));
	if (pipe(this->_stdin) == -1 || pipe(this->_stdout) == -1) {
		throw std::runtime_error("Failed to create pipe: " + std::string(std::strerror(errno)));
	}
	this->_pid = fork();
	if (this->_pid == -1) {
		throw std::runtime_error("Failed to fork: " + std::string(std::strerror(errno)));
	}
	else if (this->_pid == 0)
	{
		if (dup2(this->_stdin[PIPE_READ], STDIN_FILENO) == -1 || dup2(this->_stdout[PIPE_WRITE], STDOUT_FILENO) == -1)
			exit(1);
		close(this->_stdin[PIPE_READ]);
		close(this->_stdin[PIPE_WRITE]);
		close(this->_stdout[PIPE_READ]);
		close(this->_stdout[PIPE_WRITE]);
		std::vector<std::string> env = this->_generateForkEnv(script_path);
		std::vector<char *> env_c;
		for (size_t i = 0; i < env.size(); i++)
			env_c.push_back(const_cast<char *>(env[i].c_str()));
		env_c.push_back(NULL);
		// std::cerr << "env size: " << env_c.size() << std::endl;
		// for (size_t i = 0; i < env_c.size() - 1; i++)
		// 	std::cerr << "env[" << i << "] = " << env_c[i] << std::endl;
		std::vector<char *> argv_c;
		argv_c.push_back(const_cast<char *>(cgi_path.c_str()));
		argv_c.push_back(const_cast<char *>(script_path.c_str()));
		argv_c.push_back(NULL);
		// for (size_t i = 0; i < argv_c.size() - 1; i++)
		// 	std::cerr << "arg[" << i << "] = " << argv_c[i] << std::endl;
		execve(cgi_path.c_str(), &(argv_c.front()), &(env_c.front()));
		std::cerr << "Failed to execve: " << std::strerror(errno) << std::endl;
		exit(1);
	}
	close(this->_stdin[PIPE_READ]);
	close(this->_stdout[PIPE_WRITE]);
	this->_state = WRITE;
	if (!this->getRequest().hasBody())
	{
		this->_state = READ;
		close(this->_stdin[PIPE_WRITE]);
		this->_stdin[PIPE_WRITE] = -1;
	}
}

std::vector<std::string>	CGIHttpResponse::_generateForkEnv(const std::string &script_path) {
	std::vector<std::string> env;

	env.push_back("PATH_INFO=" + script_path);
	env.push_back("SCRIPT_FILENAME=" + script_path);
	env.push_back("REQUEST_METHOD=" + this->getRequest().getMethod());
	env.push_back("SERVER_PROTOCOL=" + std::string(HTTP_VERSION));
	env.push_back("REDIRECT_STATUS=CGI");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_SOFTWARE=webserv");
	if (!this->getRequest().getHeader("Content-Length").empty())
		env.push_back("CONTENT_LENGTH=" + this->getRequest().getHeader("Content-Length"));
	if (!this->getRequest().getHeader("Content-Type").empty())
		env.push_back("CONTENT_TYPE=" + this->getRequest().getHeader("Content-Type"));
	return env;
}

void	CGIHttpResponse::_parseHeaderLine(std::string &line) {
	size_t pos = line.find(':');
	if (pos == std::string::npos) {
		this->_state = INVALID;
		return ;
	}
	std::string key = line.substr(0, pos);
	if (key.find_first_not_of(" \t") == std::string::npos) {
		this->_state = INVALID;
		return ;
	}
	size_t value_pos = line.find_first_not_of(" \t", pos + 1);
	if (value_pos == std::string::npos)
		value_pos = 0;
	std::string value = line.substr(value_pos);
	std::cout << "Header : " << key << ": " << value << std::endl;
	this->_cgi_headers[key] = value;
}

int	CGIHttpResponse::_waitFork() {
	int status;
	waitpid(this->_pid, &status, 0);
	std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
	return WEXITSTATUS(status);
}
