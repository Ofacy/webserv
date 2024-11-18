/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 15:57:19 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/12 18:10:34 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Configuration.hpp"
#include <csignal>

void	handle_signal(int signum) {
	(void)signum;
	Configuration::exit();
}

void	setup_signal() {
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	signal(SIGQUIT, handle_signal);
}
