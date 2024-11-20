/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bind.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 17:36:38 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/20 19:50:48 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIND_HPP
# define BIND_HPP

# include "Server.hpp"
# include "Client.hpp"
# include <stdint.h>
# include <string>

class Client;
class Configuration;

class Bind : public IPollElement {
	public:
		Bind(void);
		Bind(const Bind &src);
		Bind(const std::string &host, int port);

		~Bind(void);

		Bind &operator=(const Bind &rhs);

		const std::string	&getHost(void) const;
		int					getPort(void) const;

		void				listen(void);
		Client				*accept(Configuration &config);
		
		int					getFd(void) const;
		short				getEvents(void) const;
		int					update(struct pollfd &pollfd, Configuration &config);
		AHttpResponse		*getResponse(HttpRequest &request, Configuration &config);

		void				push_server(const Server &server);
	private:
		uint32_t			_getHost(void) const;
		std::vector<Server>	_servers;
		int					_port;
		std::string			_host;
		int					_fd;

};

#endif
