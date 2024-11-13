/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:10:53 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 18:21:20 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "InheritedParameters.hpp"
# include "Location.hpp"
# include <map>
# include <string>

class Server : public InheritedParameters {
	public:
		Server(void);
		Server(const Server &src);
		Server(const Attribute &root);
		~Server(void);

		Server &operator=(const Server &rhs);

		const std::string	getHost() const;
		int					getPort() const;

		bool				hasName(const std::string &name) const;
		
		AHttpResponse		*getResponse(HttpRequest &request) const;
	private:
		bool	parseAttribute(const Attribute &child);
		
		std::string	_host;
		int			_port;

		std::vector<std::string>		_server_names;
		std::map<std::string, Location>	_locations;
};

#endif
