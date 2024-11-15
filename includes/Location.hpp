/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 16:19:55 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/15 17:38:57 by bwisniew         ###   ########.fr       */
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
		std::string					_root;
};

#endif
