/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BodyParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 15:57:05 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/20 18:59:33 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BODY_PARSER_HPP
# define BODY_PARSER_HPP

# include "HttpRequest.hpp"

class IPollElement;

class BodyParser
{
public:
	BodyParser(HttpRequest &request);
	BodyParser(const BodyParser &src);
	BodyParser &operator=(const BodyParser &src);
	virtual ~BodyParser();

	virtual IPollElement	*getPollElement();

	virtual void	update(const char *data, size_t len);
protected:
	HttpRequest		&getRequest();
private:
	HttpRequest		&_request;
	size_t			_current_body_size;
};



#endif
