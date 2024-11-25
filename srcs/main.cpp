/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 10:46:31 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/25 17:23:25 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Bind.hpp"
#include <fstream>
#include <iostream>
#include <string>

void	setup_signal();

int main(int argc, char **argv) {
	std::string config_path = "config/default.conf";

	if (argc > 2) {
		std::cerr << argv[0] << " [configfile]" << std::endl;
		return (1);
	}
	else if (argc == 2) {
		config_path = argv[1];
	}
	setup_signal();
	try {
		Configuration config(config_path);
	}
	catch(std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}