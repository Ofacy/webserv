/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:46:50 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/13 17:43:16 by bwisniew         ###   ########.fr       */
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

		const Attribute					&getRoot(void) const;

	private:
		Lexer					&_lexer;
		Attribute				_root;

		void	_parse(void);
};

std::ostream &operator<<(std::ostream &o, const Parser &rhs);

#endif
