/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 12:58:00 by bwisniew          #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"
#include <stdexcept>

Token::Token() : _type(WORD) {}

Token::Token(const Token &src) {
	*this = src;
}

Token::Token(const TokenType &type) : _type(type) {}

Token::Token(const TokenType &type, const std::string &value) : _type(type), _value(value) {
	if (_type != WORD)
		throw std::runtime_error("Token value can only be set for WORD type");
}

Token &Token::operator=(const Token &rhs) {
	if (this == &rhs)
		return (*this);
	this->_type = rhs._type;
	this->_value = rhs._value;
	return (*this);
}

Token::~Token() {}

TokenType Token::getType() const {
	return (_type);
}

const std::string &Token::getValue() const {
	return (_value);
}

std::ostream &operator<<(std::ostream &o, const Token &rhs) {
	if (rhs.getType() == WORD)
		o << "Token(" << rhs.getType() << ", " << rhs.getValue() << ")";
	else
		o << "Token(" << rhs.getType() << ")";
	return (o);
}