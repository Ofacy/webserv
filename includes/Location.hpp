/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:19:55 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 18:21:34 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <vector>
# include <string>
# include "InheritedParameters.hpp"
# include "Attribute.hpp"

class Location : public InheritedParameters {
	public:
		Location(void);
		Location(const Location &src);
		Location(const Attribute &root, const InheritedParameters &inherited);

		~Location(void);

		Location &operator=(const Location &rhs);

		const std::string	&getPath(void) const;
		AHttpResponse		*getResponse(HttpRequest &request) const;
	private:
		bool	parseAttribute(const Attribute &child);

		std::string					_path;
		size_t						_max_body_size;

		std::string					_root;
		std::vector<std::string>	_allowed_methods;
		
		bool						_autoindex;
		std::vector<std::string>	_index;

		std::map<int, std::string>	_error_pages;
		
		std::map<std::string, std::string>	_cgi_paths;
};

#endif
