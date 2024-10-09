/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 10:46:31 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
	std::string config_path = "config";

	if (argc > 2) {
		std::cerr << argv[0] << " [configfile]" << std::endl;
		return (1);
	}
	else if (argc == 2) {
		config_path = argv[1];
	}
	std::ifstream file(config_path.c_str());
	Lexer lexer(file);
	std::cout << lexer << std::endl;
	Parser parser(lexer);
	std::cout << parser << std::endl;
	return (0);
}