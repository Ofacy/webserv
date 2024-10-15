/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:46:53 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/10 16:38:31 by lcottet          ###   ########lyon.fr   */
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
	this->_attributes = rhs._attributes;
	return (*this);
}

const std::vector<Attribute> &Parser::getAttributes(void) const {
	return (this->_attributes);
}

const Attribute &Parser::getAttribute(const std::string &name) const {
	for (size_t i = 0; i < this->_attributes.size(); i++) {
		if (this->_attributes[i].getName() == name)
			return (this->_attributes[i]);
	}
	throw std::runtime_error("Attribute not found");
}

void Parser::_parse(void) {
	while (this->_lexer.getTokens().size() > 0) {
		this->_attributes.push_back(Attribute(this->_lexer));
	}
}

std::ostream &operator<<(std::ostream &o, const Parser &rhs) {
	o << "[";
	for (size_t i = 0; i < rhs.getAttributes().size(); i++) {
		o << rhs.getAttributes()[i];
		if (i + 1 < rhs.getAttributes().size())
			o << ", ";
	}
	o << "]";
	return (o);
}