/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPollElement.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 15:19:35 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 19:26:11 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APOLL_ELEMENT_HPP
# define APOLL_ELEMENT_HPP

# include <poll.h>
# include "Configuration.hpp"

class Configuration;

class IPollElement
{
public:
	virtual 		~IPollElement();
	virtual int		getFd() const = 0;
	virtual short	getEvents() const = 0;
	virtual int		update(struct pollfd &pollfd, Configuration &config) = 0;
};



#endif
