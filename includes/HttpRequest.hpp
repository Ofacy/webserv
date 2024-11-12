/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:35:52 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 18:47:52 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <sstream>
# include "HttpMessage.hpp"
class HttpRequest : public HttpMessage {
	public:
		enum HttpRequestState {
			REQUEST_LINE,
			HEADERS,
			BODY,
			DONE,
			INVALID
		};
		HttpRequest(void);
		HttpRequest(const HttpRequest &src);
		~HttpRequest(void);

		HttpRequest	&operator=(const HttpRequest &rhs);

		const std::string	&getMethod(void) const;
		const std::string	&getUri(void) const;
		const std::string	&getVersion(void) const;

		HttpRequestState	getState(void) const;
		bool				isHeaderDone(void) const;

		void				update(char *buffer, size_t size);
		bool				hasBody(void) const;
		std::string			&getBodyBuffer(void);
	private:
		void				_parseLine(std::string &line);
		void				_parseRequestLine(std::string &line);
		void				_parseHeaderLine(std::string &line);
		HttpRequestState	_state;
		std::string			_buffer;
		std::string			_body_buffer;
		std::string			_method;
		std::string			_uri;
		std::string			_version;
};

#endif