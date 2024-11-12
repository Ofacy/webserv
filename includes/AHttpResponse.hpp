/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 16:38:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/12 19:07:09 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AHTTPRESPONSE_HPP
# define AHTTPRESPONSE_HPP

# include "IPollElement.hpp"
# include <string>

class AHttpResponse 
{
public:
	virtual ~AHttpResponse();
	AHttpResponse(HttpRequest &request);
	AHttpResponse(const AHttpResponse &src);
	AHttpResponse &operator=(const AHttpResponse &rhs);

	virtual IPollElement	*getPollElement();

	virtual int				updateBodyBuffer(std::string &body_buffer);
	virtual int				writeResponse(int fd);

private:
	HttpRequest		&_request;
	std::string		_write_buffer;
	
};

#endif
