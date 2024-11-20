/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:03:25 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 17:07:134 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "AHttpResponse.hpp"
# include "HttpRequest.hpp"
# include "IPollElement.hpp"
# include <sys/socket.h>
# include <netinet/in.h>

class IPollElement;
class Configuration;
class Bind;

# define CLIENT_RECV_SIZE 65536

class Client : public IPollElement
{
public:
	enum State
	{
		READ,
		WRITE,
		READ_WRITE,
		END
	};
	Client(Bind &bind, int fd, struct sockaddr_in addr, Configuration &config);
	Client(const Client &src);
	Client &operator=(const Client &rhs);
	~Client();

	int		getFd() const;
	short	getEvents() const;
	int		update(struct pollfd &pollfd, Configuration &config);

private:
	int					_updateRead(struct pollfd &pollfd, Configuration &config);
	int					_updateWrite(struct pollfd &pollfd, Configuration &config);

	void				_cleanResponse(Configuration &config);
	Configuration		&_config;
	State				_state;
	Bind				&_bind;
	int 				_fd;
	struct sockaddr_in	_addr;
	HttpRequest			_request;
	AHttpResponse		*_response;
};




#endif
