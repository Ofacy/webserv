/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 10:46:31 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/20 19:43:10 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Bind.hpp"
#include <fstream>
#include <iostream>
#include <string>

void	setup_signal();

int main(int argc, char **argv) {
	std::string config_path = "default.conf";

	if (argc > 2) {
		std::cerr << argv[0] << " [configfile]" << std::endl;
		return (1);
	}
	else if (argc == 2) {
		config_path = argv[1];
	}
	setup_signal();
	Configuration config(config_path);
	return (0);
}