/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 12:22:06 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/15 15:25:22 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
# define TOKEN_HPP

# include <iostream>
# include <string>

enum TokenType {
	WORD,
	SEMICOLON,
	OPEN_BRACE,
	CLOSE_BRACE,
};

class Token {
	public:
		Token(void);
		Token(const Token &src);
		Token(const TokenType &type);
		Token(const TokenType &type, const std::string &value);

		~Token(void);

		Token &operator=(const Token &rhs);

		TokenType		getType(void) const;
		const std::string	&getValue(void) const;
	private:
		TokenType		_type;
		std::string		_value;
};

std::ostream &operator<<(std::ostream &o, const Token &token);

#endif
