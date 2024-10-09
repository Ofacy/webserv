/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 12:56:59 by lcottet           #+#    #+#             */
/*   Updated: 2024/10/09 16:17:45 by lcottet          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdexcept>
#include "Lexer.hpp"

const std::string Lexer::_word_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_:/.-";

Lexer::Lexer(std::ifstream &file) : _file(file) {
	this->_tokenize();
}

Lexer::Lexer(const Lexer &src) : _file(src._file) {
	*this = src;
}

Lexer::~Lexer(void) {}

Lexer &Lexer::operator=(const Lexer &rhs) {
	if (this == &rhs)
		return (*this);
	this->_tokens = rhs._tokens;
	return (*this);
}

const std::vector<Token> &Lexer::getTokens(void) const {
	return (this->_tokens);
}

Token Lexer::getNextToken(void) {
	if (this->_tokens.empty())
		throw std::runtime_error("No more tokens to read");
	Token ret = this->_tokens.front();
	this->_tokens.erase(this->_tokens.begin());
	return (ret);
}

const Token &Lexer::peekToken(void) const {
	if (this->_tokens.empty())
		throw std::runtime_error("No more tokens to read");
	return (this->_tokens.front());
}

void Lexer::_tokenize(void) {
	std::string str_tok;
	while (!this->_file.eof()) {
		str_tok.clear();
		this->_file >> str_tok;
		std::string word;
		for (size_t i = 0; i < str_tok.size(); i++) {
			TokenType type = this->_getTokenType(str_tok[i]);
			if (type == WORD)
				word = word + str_tok[i];
			else
			{
				if (!word.empty())
					this->_tokens.push_back(Token(WORD, word));
				this->_tokens.push_back(Token(type));
				word.clear();
			}
		}
		if (!word.empty())
			this->_tokens.push_back(Token(WORD, word));
	}
}

TokenType	Lexer::_getTokenType(char c) const {
	if (c == ';')
		return (SEMICOLON);
	if (c == '{')
		return (OPEN_BRACE);
	if (c == '}')
		return (CLOSE_BRACE);
	if (this->_word_chars.find(c) == std::string::npos)
		throw std::runtime_error("Invalid character in input");
	return (WORD);
}

std::ostream &operator<<(std::ostream &file, const Lexer &lexer) {
	const std::vector<Token> &tokens = lexer.getTokens();
	for (size_t i = 0; i < tokens.size(); i++) {
		file << tokens[i] << std::endl;
	}
	return (file);
}