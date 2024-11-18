/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHttpResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 14:52:00 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/18 20:38:54 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHTTPRESPONSE_HPP
# define CGIHTTPRESPONSE_HPP

# include <string>
# include <map>
# include <vector>
# include "AHttpResponse.hpp"
# include "IPollElement.hpp"

# define SOCKET_PARENT 0
# define SOCKET_CHILD 1

# define CGI_READ_BUFFER_SIZE 65536

class CGIHttpResponse : public AHttpResponse, public IPollElement {
public:

	CGIHttpResponse(HttpRequest &request, const std::string &script_path, const std::string &cgi_path);
	CGIHttpResponse(const CGIHttpResponse &src);
	~CGIHttpResponse(void);

	CGIHttpResponse &operator=(const CGIHttpResponse &rhs);
	
	IPollElement	*getPollElement();
	int				getFd() const;
	short			getEvents() const;
	int				update(struct pollfd &pollfd, Configuration &config);
private:
	int							_readCGI(struct pollfd &pollfd);
	int							_writeCGI(struct pollfd &pollfd);
	void						_finishHeader();
	void						_forkCGI(const std::string &script_path, const std::string &cgi_path);
	std::vector<std::string>	_generateForkEnv(const std::string &script_path);
	void						_parseHeaderLine(struct pollfd &pollfd, std::string &line);
	int							_waitFork();
	void						_finish(struct pollfd &pollfd);

	std::map<std::string, std::string>	_cgi_headers;
	std::string							_read_buffer;
	int									_pid;
	int									_fd;
};

#endif
