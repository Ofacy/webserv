/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:08:06 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/26 17:59:26 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "IPollElement.hpp"
# include "Bind.hpp"
# include "Server.hpp"
# include <vector>
# include <string>

# define DEFAULT_MAX_HEADER_SIZE 8192

class Client;

class Configuration : public InheritedParameters {
	public:
		Configuration(void);
		Configuration(const Configuration &src);
		Configuration(const std::string &config_path);
		~Configuration(void);

		Configuration	&operator=(const Configuration &rhs);
		void			addPollElement(IPollElement *poll_element);
		void			removePollElement(IPollElement *poll_element);
		size_t			getClientMaxHeaderSize(void) const;
		static 			void	exit();

		void			start();
	private:
		bool						parseAttribute(const Attribute &child);
		void						_assignServer(const Attribute &server_attribute);
		Bind						&_getBind(const Server &server);

		std::vector<struct pollfd>	_pollfds;
		std::vector<IPollElement *>	_poll_elements;
		std::vector<Bind *>			_binds;
		size_t						_polli;
		size_t						_client_max_header_size;
		static bool					_exit;
};

#endif
