/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bind.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 17:36:38 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/10 18:11:32 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIND_HPP
# define BIND_HPP

# include "Server.hpp"
# include <stdint.h>
# include <string>

class Bind {
	public:
		Bind(void);
		Bind(const Bind &src);
		Bind(const std::string &host, int port);

		~Bind(void);

		Bind &operator=(const Bind &rhs);

		const std::string	&getHost(void) const;
		int					getPort(void) const;

		void				listen(void);

		void				push_server(const Server &server);
	private:
		uint32_t			_getHost(void) const;
		std::vector<Server>	_servers;
		int					_port;
		std::string			_host;
		int					_fd;

};

#endif
