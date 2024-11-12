/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 16:38:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/12 19:26:28 by bwisniew         ###   ########.fr       */
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
	virtual int				writeResponse(int fd);
protected:
	HttpRequest		&getRequest();
	void			setBufferDone(bool done);
	bool			isBufferDone() const;
	bool			isHeaderReady() const = 0;
private:
	HttpRequest		&_request;
	std::string		_write_buffer;
	bool			_buffer_done;
	
};

#endif
