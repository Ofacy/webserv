/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHttpResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 14:52:00 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/16 17:29:02 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHTTPRESPONSE_HPP
# define CGIHTTPRESPONSE_HPP

# include <string>
# include <map>
# include <vector>
# include "AHttpResponse.hpp"
# include "IPollElement.hpp"

# define PIPE_READ 0
# define PIPE_WRITE 1

# define CGI_READ_BUFFER_SIZE 4096

class CGIHttpResponse : public AHttpResponse, public IPollElement {
public:
	enum State {
		WRITE,
		READ,
		DONE,
		INVALID
	};

	CGIHttpResponse(HttpRequest &request, const std::string &script_path, const std::string &cgi_path);
	CGIHttpResponse(const CGIHttpResponse &src);
	~CGIHttpResponse(void);

	CGIHttpResponse &operator=(const CGIHttpResponse &rhs);
	
	IPollElement	*getPollElement();
	int				getFd() const;
	short			getEvents() const;
	int				update(struct pollfd &pollfd, Configuration &config);
private:
	int							_readCGI();
	int							_writeCGI(struct pollfd &pollfd);
	void						_finishHeader();
	void						_forkCGI(const std::string &script_path, const std::string &cgi_path);
	std::vector<std::string>	_generateForkEnv(const std::string &script_path);
	void						_parseHeaderLine(std::string &line);
	int							_waitFork();

	std::map<std::string, std::string>	_cgi_headers;
	std::string							_read_buffer;
	State								_state;
	int									_pid;
	int									_stdin[2];
	int									_stdout[2];
};

#endif
