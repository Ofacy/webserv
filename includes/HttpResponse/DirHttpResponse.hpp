/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirHttpResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 18:45:42 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/15 19:52:18 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRHTTPRESPONSE_HPP
# define DIRHTTPRESPONSE_HPP

# include "AHttpResponse.hpp"
# include <stdint.h>

# define HTML_DIR_BEGIN "<!DOCTYPE html>\n\
<html lang=\"fr\">\n\
<head>\n\
    <meta charset=\"UTF-8\">\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    <title>Directory listing</title>\n\
    <style>\n\
        body {\n\
            margin: 0;\n\
            padding: 0;\n\
            font-family: 'Arial', sans-serif;\n\
            background-color: #f0f0f0;\n\
            display: flex;\n\
            justify-content: center;\n\
            align-items: center;\n\
            height: 100vh;\n\
            width: 100vw;\n\
            overflow: hidden;\n\
        }\n\
    </style>\n\
</head>\n\
<body>\n\
    <ul>\n"
	
# define HTML_DIR_END "	</ul>\n\
</body>\n\
</html>\n"

class DirHttpResponse : public AHttpResponse {
	public:
		DirHttpResponse(const DirHttpResponse & src);
		DirHttpResponse(HttpRequest &request, uint16_t status, const std::string &path);
		DirHttpResponse & operator=(const DirHttpResponse & src);
		~DirHttpResponse(void);
	private:
		std::string	_buildDirectoryElement(const std::string &file, const std::string &path);
};

#endif
