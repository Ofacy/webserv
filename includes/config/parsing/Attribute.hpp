/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Attribute.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 12:18:42 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATTRIBUTE_HPP
# define ATTRIBUTE_HPP

# include "Lexer.hpp"
# include <iostream>
# include <vector>
# include <string>

class Attribute {
	public:
		Attribute(void);
		Attribute(const Attribute &src);
		Attribute(Lexer &lexer);

		~Attribute(void);

		Attribute &operator=(const Attribute &rhs);

		const std::string				&getName(void) const;
		const std::vector<std::string>	&getParameters(void) const;
		const std::vector<Attribute>	&getChildren(void) const;
	private:
		std::string					_name;
		std::vector<std::string>	_parameters;
		std::vector<Attribute>		_children;
};

std::ostream &operator<<(std::ostream &o, const Attribute &attr);

/*
server {
	host ofacy.com;
	location / {
		satisfy any;

		allow 192.168.1.0/32;
		deny  all;
	}
	server_name	www.example.com;
}

*/

#endif
