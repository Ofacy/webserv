/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:46:53 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 17:43:39 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(Lexer &lexer) : _lexer(lexer) {
	this->_parse();
}

Parser::Parser(const Parser &src) : _lexer(src._lexer) {
	*this = src;
}

Parser::~Parser(void) {}

Parser &Parser::operator=(const Parser &rhs) {
	if (this == &rhs)
		return (*this);
	this->_root = rhs._root;
	return (*this);
}

const Attribute &Parser::getRoot(void) const {
	return (this->_root);
}

void Parser::_parse(void) {
	while (this->_lexer.getTokens().size() > 0) {
		this->_root.pushChild(Attribute(this->_lexer));
	}
}

std::ostream &operator<<(std::ostream &o, const Parser &rhs) {
	o << rhs.getRoot();
	return (o);
}