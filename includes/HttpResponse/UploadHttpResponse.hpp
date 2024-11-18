/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHttpResponse.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 14:56:41 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/18 20:35:42 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADHTTPRESPONSE_HPP
# define UPLOADHTTPRESPONSE_HPP

# include "AHttpResponse.hpp"
# include "IPollElement.hpp"

class UploadHttpResponse : public AHttpResponse, public IPollElement {
public:
	UploadHttpResponse(HttpRequest &request, const std::string &path);
	UploadHttpResponse(UploadHttpResponse const & src);
	~UploadHttpResponse(void);

	UploadHttpResponse & operator=(UploadHttpResponse const & rhs);

	IPollElement *getPollElement();

	int			getFd() const;
	short		getEvents() const;
	int			update(struct pollfd &pollfd, Configuration &config);
private:
	int	_fd;
};


#endif
