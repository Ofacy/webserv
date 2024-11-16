/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirHttpResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 18:45:42 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/16 22:55:05 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRHTTPRESPONSE_HPP
# define DIRHTTPRESPONSE_HPP

# include "AHttpResponse.hpp"
# include <stdint.h>
# include <sstream>

# define HTML_DIR "<!DOCTYPE html>\n\
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
            min-height: 100vh;\n\
        }\n\
        .container {\n\
            text-align: center;\n\
            color: #333;\n\
            width: 90%;\n\
            max-width: 800px;\n\
            background: white;\n\
            padding: 20px;\n\
            border-radius: 10px;\n\
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n\
        }\n\
        h1 {\n\
            font-size: 2.5rem;\n\
            margin-bottom: 20px;\n\
        }\n\
        table {\n\
            width: 100%;\n\
            border-collapse: collapse;\n\
            margin-top: 20px;\n\
        }\n\
        th, td {\n\
            padding: 15px;\n\
            text-align: left;\n\
            border: 1px solid #ddd;\n\
        }\n\
        th {\n\
            background-color: rgb(85, 85, 173);\n\
            color: white;\n\
        }\n\
        tr:nth-child(even) {\n\
            background-color: #f9f9f9;\n\
        }\n\
        tr:hover {\n\
            background-color: #f1f1f1;\n\
        }\n\
        .icon {\n\
            margin-right: 8px;\n\
        }\n\
        .folder {\n\
            color: rgb(42, 42, 85);\n\
            font-weight: bold;\n\
        }\n\
        .btn {\n\
            display: inline-block;\n\
            padding: 10px 20px;\n\
            margin-top: 20px;\n\
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
    <div class=\"container\">\n\
        <h1>Index of #path#</h1>\n\
        <table>\n\
            <thead>\n\
                <tr>\n\
                    <th>Nom</th>\n\
                    <th>Taille</th>\n\
                </tr>\n\
            </thead>\n\
            <tbody>#table#</tbody>\n\
        </table>\n\
        <a href=\"/\" class=\"btn\">Back to home</a>\n\
    </div>\n\
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
        void        _buildFileRow(std::stringstream &ss, const std::string &file, const long size);
        void        _buildDirRow(std::stringstream &ss, const std::string &file);
        std::string _getUriPath(const std::string &file);
};

#endif
