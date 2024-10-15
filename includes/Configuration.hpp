/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:08:06 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/15 18:26:32 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "Bind.hpp"
# include "Server.hpp"
# include <vector>
# include <string>

class Configuration {
	public:
		Configuration(void);
		Configuration(const Configuration &src);
		Configuration(const std::string &config_path);
		~Configuration(void);

		Configuration &operator=(const Configuration &rhs);

	private:
		
		void				_assignServer(const Attribute &server_attribute);
		Bind				&_getBind(const Server &server);
		std::vector<Bind>	_binds;
};

#endif
