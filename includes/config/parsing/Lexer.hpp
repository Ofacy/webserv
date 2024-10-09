/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 12:21:26 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "Token.hpp"

class Lexer {
	public:
		Lexer(std::ifstream &file);
		Lexer(const Lexer &src);

		~Lexer(void);

		Lexer &operator=(const Lexer &rhs);

		const std::vector<Token>	&getTokens(void) const;
		Token						getNextToken(void);

		const Token					&peekToken(void) const;
	private:
		const static std::string	_word_chars;
		std::vector<Token>			_tokens;
		std::ifstream				&_file;

		void		_tokenize(void);
		TokenType	_getTokenType(char c) const;
};

std::ostream &operator<<(std::ostream &o, const Lexer &lexer);

#endif
