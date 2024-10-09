/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:10:53 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Attribute.hpp"
# include <string>

class Server {
	public:
		Server(void);
		Server(const Server &src);
		Server(const Attribute &attr);
		~Server(void);

		Server &operator=(const Server &rhs);

	private:
		std::string _host;
		int			_port;
		std::string _server_name;
		std::string _error_page;
};

#endif
