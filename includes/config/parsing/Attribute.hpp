/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Attribute.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 12:18:42 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/13 17:44:16 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATTRIBUTE_HPP
# define ATTRIBUTE_HPP

# include "Lexer.hpp"
# include <map>
# include <iostream>
# include <vector>
# include <string>

class Attribute {
	public:
		Attribute(void);
		Attribute(const Attribute &src);
		Attribute(Lexer &lexer);

		~Attribute(void);

		Attribute &operator=(const Attribute &rhs);

		const std::string										&getName(void) const;
		const std::vector<std::string>							&getParameters(void) const;
		const std::vector<std::string>							&getParameters(size_t assert_count) const;

		const std::vector<Attribute>							&getChildren(void) const;

		void													pushChild(const Attribute &child);

		void													assertNoChild() const;
	private:
		std::string					_name;
		std::vector<std::string>	_parameters;
		std::vector<Attribute>		_children;
};

std::ostream &operator<<(std::ostream &o, const Attribute &attr);

#endif
