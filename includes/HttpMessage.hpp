/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 21:32:46 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 19:07:40 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

# include <map>
# include <string>


class HttpMessage {
	public:
		HttpMessage(void);
		HttpMessage(const HttpMessage &src);
		~HttpMessage(void);

		HttpMessage	&operator=(const HttpMessage &rhs);

		std::map<std::string, std::string>	&getHeaders(void);
		std::string							getHeader(const std::string &key) const;
		const std::string					&getBody(void) const;

		void								setHeader(const std::string &key, const std::string &value);
		void								setBody(const std::string &body);

	private:
		std::map<std::string, std::string>	_headers;
};

#endif