/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:03:25 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/12 19:07:40 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Bind.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include "IPollElement.hpp"
# include "HttpRequest.hpp"
# include "AHttpResponse.hpp"

class Bind;

#define CLIENT_RECV_SIZE 4096

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
	Client(Bind &bind, int fd, struct sockaddr_in addr);
	Client(const Client &src);
	Client &operator=(const Client &rhs);
	~Client();

	int		getFd() const;
	short	getEvents() const;
	int		update(struct pollfd &pollfd, Configuration &config);

private:
	int					_updateRead(struct pollfd &pollfd, Configuration &config);

	State				_state;
	Bind				&_bind;
	int 				_fd;
	struct sockaddr_in	_addr;
	HttpRequest			_request;
	AHttpResponse		*_response;
};




#endif
