/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAttributeParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 17:24:44 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 17:58:49 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AAttributeParser.hpp"
#include <algorithm>

AAttributeParser::AAttributeParser(void) {}

AAttributeParser::AAttributeParser(const AAttributeParser &src) {
	*this = src;
}

AAttributeParser & AAttributeParser::operator=(const AAttributeParser & rhs)
{
	if (this == &rhs)
		return (*this);
	this->_object_name = rhs._object_name;
	return (*this);
}

AAttributeParser::~AAttributeParser(void) {}

void	AAttributeParser::parse(const Attribute &root, std::vector<std::string> mandatory_childs) {
	this->_object_name = root.getName();
	for(std::vector<Attribute>::const_iterator it = root.getChildren().begin(); it != root.getChildren().end(); it++){
		if (!this->parseAttribute(*it))
			throw InvalidAttributeException(it->getName(), this->_object_name);
		this->_defined_children.push_back(it->getName());
	}
	for (std::vector<std::string>::const_iterator it = mandatory_childs.begin(); it != mandatory_childs.end(); it++) {
		if (std::find(this->_defined_children.begin(), this->_defined_children.end(), *it) == this->_defined_children.end())
			throw NotDefinedException(*it, this->_object_name);
	}
	this->_defined_children.clear();
}

void	AAttributeParser::assertNotAlreadyDefined(const std::string &name) const {
	if (std::find(this->_defined_children.begin(), this->_defined_children.end(), name) != this->_defined_children.end())
		throw AlreadyDefinedException(name, this->_object_name);
}

AAttributeParser::AlreadyDefinedException::AlreadyDefinedException(const std::string &name, const std::string &object_name) : std::runtime_error("Attribute " + name + " defined more than once in the same object " + object_name) {}

AAttributeParser::NotDefinedException::NotDefinedException(const std::string &name, const std::string &object_name) : std::runtime_error("Mandatory attribute " + name + " not defined in object " + object_name) {}

AAttributeParser::InvalidAttributeException::InvalidAttributeException(const std::string &name, const std::string &object_name) : std::runtime_error("Invalid attribute " + name + " in object " + object_name) {}
