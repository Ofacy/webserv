/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusHttpResponse.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:43:52 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/18 20:38:54 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSHTTPRESPONSE_HPP
# define STATUSHTTPRESPONSE_HPP

# include "AHttpResponse.hpp"

class StatusHttpResponse : public AHttpResponse {
public:
	StatusHttpResponse(HttpRequest &request, uint16_t code, const std::string &body);
	StatusHttpResponse(HttpRequest &request, uint16_t code);
	StatusHttpResponse(StatusHttpResponse const & src);
	~StatusHttpResponse(void);

	StatusHttpResponse & operator=(StatusHttpResponse const & rhs);
};

#endif
