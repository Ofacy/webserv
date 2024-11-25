/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AccessLog.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 16:25:41 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/25 17:23:25 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCESSLOG_HPP
# define ACCESSLOG_HPP

# include "IPollElement.hpp"
# include <string>

class Client;

class AccessLog : public IPollElement {
public:
	static AccessLog	*&getInstance(const std::string &path);
	static AccessLog	*&getInstance();
	~AccessLog();

	int		getFd() const;
	short	getEvents() const;
	int		update(struct pollfd &pollfd, Configuration &config);

	AccessLog 	&operator<<(const std::string &str);
	AccessLog	&operator<<(const Client &client);
private:
	AccessLog(const std::string &path);
	static AccessLog *_instance;
	std::string		_getTime() const;
	std::string		_getResponseStr(const Client &client) const;

	int			_fd;
	std::string _buffer;
};



#endif
