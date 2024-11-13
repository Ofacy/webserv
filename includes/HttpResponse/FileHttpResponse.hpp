/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHttpResponse.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 18:27:55 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/13 18:34:07 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEHTTPRESPONSE_HPP
# define FILEHTTPRESPONSE_HPP

# include <sys/stat.h>
# include "AHttpResponse.hpp"
# include "IPollElement.hpp"

class FileHttpResponse : public AHttpResponse, public IPollElement
{
public:
	~FileHttpResponse();
	FileHttpResponse();
	FileHttpResponse(const FileHttpResponse &src);
	FileHttpResponse(HttpRequest &request, int fd, struct stat stats);
	FileHttpResponse &operator=(const FileHttpResponse &src);

	
};



#endif
