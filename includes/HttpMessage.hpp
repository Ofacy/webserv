/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:32:46 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/16 17:29:02 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

# include <map>
# include <string>

#define HTTP_VERSION "HTTP/1.1"


class HttpMessage {
	public:
		HttpMessage(void);
		HttpMessage(const HttpMessage &src);
		~HttpMessage(void);

		HttpMessage	&operator=(const HttpMessage &rhs);

		std::map<std::string, std::string>	&getHeaders(void);
		std::string							getHeader(const std::string &key) const;

		void								setHeader(const std::string &key, const std::string &value);

	private:
		std::map<std::string, std::string>	_headers;
};

#endif