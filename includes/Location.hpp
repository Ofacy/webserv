/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:19:55 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/15 18:19:38 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <vector>
# include <string>
# include "AAttributeParser.hpp"
# include "Attribute.hpp"

class Location : public AAttributeParser {
	public:
		Location(void);
		Location(const Location &src);
		Location(const Attribute &root);

		~Location(void);

		Location &operator=(const Location &rhs);

		const std::string	&getPath(void) const;

	private:
		void	parseAttribute(const Attribute &child);

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
