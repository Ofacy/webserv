/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:09:24 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <fstream>

Configuration::Configuration(void) {};

Configuration::Configuration(const Configuration &src) {
	*this = src;
}

Configuration::Configuration(const std::string &config_path) {
	std::ifstream file(config_path);

	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file");
	}
	Lexer lexer(file);
	Parser parser(lexer);
	for (size_t i = 0; i < parser.getAttributes().size(); i++) {
		if (parser.getAttributes()[i].getName() == "server") {
			_servers.push_back(Server(parser.getAttributes()[i]));
		}
		else
			throw std::runtime_error("Configuration file contains unknown attribute (expecting only 'server' on root)");
	}
}

Configuration::~Configuration(void) {};