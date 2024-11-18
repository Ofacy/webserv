/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAttributeParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 17:13:04 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 17:58:23 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATTRIBUTE_PARSER_HPP
# define ATTRIBUTE_PARSER_HPP

# include "Attribute.hpp"

class AAttributeParser {
	protected:
		AAttributeParser(void);
		AAttributeParser(const AAttributeParser &src);
		AAttributeParser &operator=(const AAttributeParser &rhs);

		virtual ~AAttributeParser(void);

		void			parse(const Attribute &root, std::vector<std::string> mandatory_childs);
		virtual bool	parseAttribute(const Attribute &child) = 0;
		void			assertNotAlreadyDefined(const std::string &name) const;

		class AlreadyDefinedException : public std::runtime_error {
			public:
				AlreadyDefinedException(const std::string &name, const std::string &object_name);
		};
		class NotDefinedException : public std::runtime_error {
			public:
				NotDefinedException(const std::string &name, const std::string &object_name);
		};
		class InvalidAttributeException : public std::runtime_error {
			public:
				InvalidAttributeException(const std::string &name, const std::string &object_name);
		};
	private:
		std::vector<std::string>	_defined_children;
		std::string					_object_name;
};

#endif
