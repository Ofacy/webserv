/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:08:06 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 19:26:11 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "Bind.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "IPollElement.hpp"
# include <vector>
# include <string>

class IPollElement;
class Bind;

class Configuration {
	public:
		Configuration(void);
		Configuration(const Configuration &src);
		Configuration(const std::string &config_path);
		~Configuration(void);

		Configuration	&operator=(const Configuration &rhs);
		void			addPollElement(IPollElement *poll_element);
		static 			void	exit();

	private:
		
		void						_assignServer(const Attribute &server_attribute);
		Bind						&_getBind(const Server &server);

		void						_poll();
		std::vector<struct pollfd>	_pollfds;
		std::vector<IPollElement *>	_poll_elements;
		std::vector<Bind *>			_binds;
		static bool					_exit;
};

#endif
