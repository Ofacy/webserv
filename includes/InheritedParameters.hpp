/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InheritedParameters.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 17:30:26 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/25 23:30:03 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef INHERITEDPARAMETERS_HPP
# define INHERITEDPARAMETERS_HPP

# include "AAttributeParser.hpp"
# include "HttpRequest.hpp"
# include "AHttpResponse.hpp"

# define NB_SUPPORTED_METHODS 5

class InheritedParameters : public AAttributeParser {
	public:
		InheritedParameters(void);
		InheritedParameters(const InheritedParameters &src);
		~InheritedParameters(void);

		InheritedParameters	&operator=(const InheritedParameters &rhs);

		virtual bool		parseAttribute(const Attribute &child);

		size_t										getMaxBodySize(void) const;
		const std::vector<std::string>				&getAllowedMethods(void) const;
		bool										getAutoindex(void) const;
		const std::vector<std::string>				&getIndex(void) const;
		const std::map<uint16_t, std::string>		&getErrorPages(void) const;
		const std::map<std::string, std::string>	&getCgiPaths(void) const;
		const std::pair<uint16_t, std::string>		&getReturn(void) const;

		AHttpResponse							*prepareResponse(HttpRequest &request, const std::string &root, const std::string &uri) const;
		AHttpResponse							*getErrorResponse(HttpRequest &request, const uint16_t status_code, const std::string &root) const;
		AHttpResponse							*getErrorResponse(HttpRequest &request, const uint16_t status_code) const;
	private:
		AHttpResponse						*_getDirectoryResponse(HttpRequest &request, const std::string &path, const std::string &root) const;
		AHttpResponse						*_getCGIResponse(HttpRequest &request, const std::string &path) const;
		const static std::string			_supported_methods[NB_SUPPORTED_METHODS];

		std::pair<uint16_t, std::string>	_return;
		std::string							_upload_folder;
		size_t								_max_body_size;
		std::vector<std::string>			_allowed_methods;
		bool								_autoindex;
		std::vector<std::string>			_index;
		std::map<uint16_t, std::string>		_error_pages;
		std::map<std::string, std::string>	_cgi_paths;
};

#endif
