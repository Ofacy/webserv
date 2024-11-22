/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusHttpResponse.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 16:43:52 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/22 11:10:56 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSHTTPRESPONSE_HPP
# define STATUSHTTPRESPONSE_HPP

# include "AHttpResponse.hpp"

# define STATUS_PAGE "<!DOCTYPE html>\n\
<html lang=\"fr\">\n\
<head>\n\
    <meta charset=\"UTF-8\">\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    <title>#title#</title>\n\
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
        .container {\n\
            text-align: center;\n\
            color: #333;\n\
			position: relative;\n\
			width: 100%;\n\
			height: 100%;\n\
            display: flex;\n\
            justify-content: center;\n\
            align-items: center;\n\
            flex-direction: column;\n\
        }\n\
        h1 {\n\
            font-size: 5rem;\n\
            margin: 0;\n\
        }\n\
        p {\n\
            font-size: 1.2rem;\n\
            margin-top: 10px;\n\
            color: #666;\n\
        }\n\
        .btn {\n\
            display: inline-block;\n\
            padding: 15px 30px;\n\
            margin-top: 30px;\n\
            font-size: 1rem;\n\
            color: white;\n\
            background-color: rgb(85, 85, 173);\n\
            text-decoration: none;\n\
            border-radius: 5px;\n\
            transition: background-color 0.3s ease;\n\
        }\n\
        .btn:hover {\n\
            background-color: rgb(42, 42, 85);\n\
        }\n\
    </style>\n\
</head>\n\
<body>\n\
    <div class=\"container\" id=\"container\">\n\
        <h1>#code#</h1>\n\
        <p>#message#.</p>\n\
        <a href=\"/\" class=\"btn\">Back to home</a>\n\
    </div>\n\
</body>\n\
</html>"

class StatusHttpResponse : public AHttpResponse {
public:
	StatusHttpResponse(HttpRequest &request, uint16_t code, const std::string &body);
	StatusHttpResponse(HttpRequest &request, uint16_t code);
	StatusHttpResponse(StatusHttpResponse const & src);
	~StatusHttpResponse(void);

	StatusHttpResponse & operator=(StatusHttpResponse const & rhs);
};

#endif
