/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Attribute.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:55:08 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 14:27:20 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Attribute.hpp"

Attribute::Attribute(void) {}

Attribute::Attribute(const Attribute &src) {
	*this = src;
}

Attribute &Attribute::operator=(const Attribute &src) {
	if (this == &src)
		return *this;
	this->_name = src._name;
	this->_parameters = src._parameters;
	this->_children = src._children;
	return *this;
}

Attribute::Attribute(Lexer &lexer) {
	Token token = lexer.getNextToken();
	if (token.getType() != WORD)
		throw std::runtime_error("Attribute name must be a WORD");
	this->_name = token.getValue();
	for (token = lexer.getNextToken(); token.getType() != SEMICOLON && token.getType() != OPEN_BRACE; token = lexer.getNextToken()) {
		if (token.getType() != WORD)
			throw std::runtime_error("Attribute parameter must be a WORD");
		this->_parameters.push_back(token.getValue());
	}
	if (token.getType() == OPEN_BRACE) {
		for (token = lexer.peekToken(); token.getType() != CLOSE_BRACE; token = lexer.peekToken()) {
			if (token.getType() != WORD)
				throw std::runtime_error("Attribute child name must be a WORD");
			this->_children.push_back(Attribute(lexer));
		}
		lexer.getNextToken();
	}
}

Attribute::~Attribute(void) {}

const std::string &Attribute::getName(void) const {
	return (this->_name);
}

const std::vector<std::string> &Attribute::getParameters(void) const {
	return (this->_parameters);
}

const std::vector<Attribute> &Attribute::getChildren(void) const {
	return (this->_children);
}

std::ostream &operator<<(std::ostream &o, const Attribute &attr) {
	o << "{\"name\": \"" << attr.getName() << "\", \"parameters\": [";
	for (size_t i = 0; i < attr.getParameters().size(); i++) {
		o << "\"" << attr.getParameters()[i] << "\"";
		if (i + 1 < attr.getParameters().size())
			o << ", ";
	}
	if (attr.getChildren().size() != 0) {
		o << "], \"children\": [";
		for (size_t i = 0; i < attr.getChildren().size(); i++) {
			o << attr.getChildren()[i];
			if (i + 1 < attr.getChildren().size())
				o << ", ";
		}
	}
	o << "]}";
	return (o);
}
