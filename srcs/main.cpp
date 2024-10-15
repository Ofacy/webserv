/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 10:46:31 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/15 18:30:46 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Bind.hpp"
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
	Configuration config(config_path);
	return (0);
}