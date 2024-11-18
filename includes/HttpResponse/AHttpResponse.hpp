/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 16:38:36 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/18 20:38:54 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef AHTTPRESPONSE_HPP
# define AHTTPRESPONSE_HPP

# include <string>
# include <stdint.h>
# include "HttpRequest.hpp"

class IPollElement;

class AHttpResponse 
{
public:
	virtual ~AHttpResponse();
	AHttpResponse(HttpRequest &request);
	AHttpResponse(const AHttpResponse &src);
	AHttpResponse &operator=(const AHttpResponse &rhs);
	AHttpResponse &operator<<(const std::string &str);

	virtual IPollElement	*getPollElement();
	virtual int				writeResponse(int fd);
protected:
	HttpRequest		&getRequest();
	void			setBufferDone(bool done);
	bool			isBufferDone() const;
	bool			isHeaderReady() const;

	void			createHeaderBuffer(uint16_t code, const std::map<std::string, std::string> &headers);
	std::string		getReasonPhrase(uint16_t code) const;
	void			appendBody(char *data, size_t size);
	void			appendBody(const std::string &data);
private:
	HttpRequest		&_request;
	std::string		_write_buffer;
	bool			_buffer_done;
	bool			_header_ready;
	
};

#endif
