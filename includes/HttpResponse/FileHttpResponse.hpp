/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHttpResponse.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 18:27:55 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/18 20:38:54 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEHTTPRESPONSE_HPP
# define FILEHTTPRESPONSE_HPP

# include <sys/stat.h>
# include "AHttpResponse.hpp"
# include "IPollElement.hpp"

#define FILE_BUFFER_SIZE 65536

class FileHttpResponse : public AHttpResponse, public IPollElement
{
public:
	FileHttpResponse();
	FileHttpResponse(const FileHttpResponse &src);
	FileHttpResponse(HttpRequest &request, uint16_t status, int fd, struct stat stats);
	FileHttpResponse &operator=(const FileHttpResponse &src);
	~FileHttpResponse();

	IPollElement *getPollElement();

	int		getFd() const;
	short	getEvents() const;
	int		update(struct pollfd &pollfd, Configuration &config);
private:
	int			_fd;

};



#endif
