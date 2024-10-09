/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:46:50 by bwisniew          #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <vector>

# include "Attribute.hpp"
# include "Lexer.hpp"
# include <iostream>

class Parser
{
	public:
		Parser(Lexer &lexer);
		Parser(const Parser &src);
		
		~Parser(void);

		Parser &operator=(const Parser &rhs);

		const std::vector<Attribute>	&getAttributes(void) const;
		const Attribute 				&getAttribute(const std::string &name) const;

	private:
		Lexer					&_lexer;
		std::vector<Attribute>	_attributes;

		void	_parse(void);
};

std::ostream &operator<<(std::ostream &o, const Parser &rhs);

#endif
