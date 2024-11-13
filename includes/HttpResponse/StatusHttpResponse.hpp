/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusHttpResponse.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:43:52 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 16:53:32 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSHTTPRESPONSE_HPP
# define STATUSHTTPRESPONSE_HPP

# include "AHttpResponse.hpp"

class StatusHttpResponse : public AHttpResponse {
public:
	StatusHttpResponse(HttpRequest &request, uint16_t code);
	StatusHttpResponse(StatusHttpResponse const & src);
	~StatusHttpResponse(void);

	StatusHttpResponse & operator=(StatusHttpResponse const & rhs);
};

#endif
