/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPollElement.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 15:19:35 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 17:11:12 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IPOLL_ELEMENT_HPP
# define IPOLL_ELEMENT_HPP

# include <poll.h>

class Configuration;

class IPollElement
{
public:
	virtual 		~IPollElement() {};
	virtual int		getFd() const = 0;
	virtual short	getEvents() const = 0;
	virtual int		update(struct pollfd &pollfd, Configuration &config) = 0;
};

#endif
