/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:35:52 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/25 21:32:05 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <sstream>
# include "HttpMessage.hpp"

class BodyParser;
class Configuration;

class HttpRequest : public HttpMessage {
	public:
		enum HttpRequestState {
			REQUEST_LINE,
			HEADERS,
			PARSING_BODY,
			BODY,
			DONE,
			INVALID,
			URI_TOO_LONG
		};
		HttpRequest(Configuration &config);
		HttpRequest(const HttpRequest &src);
		~HttpRequest(void);

		HttpRequest	&operator=(const HttpRequest &rhs);

		const std::string	&getMethod(void) const;
		const std::string	&getUri(void) const;
		const std::string	&getVersion(void) const;
		size_t				getContentLength(void) const;
		void				setContentLength(size_t content_length);
		size_t				getMaxBodySize(void) const;
		void				setMaxBodySize(size_t max_body_size);

		HttpRequestState	getState(void) const;
		void				setState(HttpRequestState state);
		bool				isDone(void) const;
		bool				isHeaderDone(void) const;

		void				update(char *buffer, size_t size);
		bool				hasBody(void) const;
		std::string			&getBodyBuffer(void);
	private:
		void				_parseLine(std::string &line);
		void				_parseRequestLine(std::string &line);
		void				_parseHeaderLine(std::string &line);\
		BodyParser			*_selectBodyParser(void);

		Configuration		&_config;
		HttpRequestState	_state;
		BodyParser			*_body_parser;
		std::string			_buffer;
		std::string			_body_buffer;
		size_t				_max_body_size;
		size_t				_content_length;
		std::string			_method;
		std::string			_uri;
		std::string			_version;
};

#endif