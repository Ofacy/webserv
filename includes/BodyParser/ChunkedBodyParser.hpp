/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedBodyParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 16:01:31 by bwisniew          #+#    #+#             */
/*   Updated: 2024/11/20 19:30:52 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHUNKED_BODY_PARSER_HPP
# define CHUNKED_BODY_PARSER_HPP

# include "BodyParser.hpp"
# include "IPollElement.hpp"

# define FILE_READ_SIZE 4096

class ChunkedBodyParser : public BodyParser, public IPollElement {
public:
	enum State {
		READ_CHUNK_SIZE,
		READ_CHUNK,
		READ_CHUNK_END,
		WRITE_RESPONSE,
		INVALID
	};

	ChunkedBodyParser(HttpRequest &request);
	ChunkedBodyParser(const ChunkedBodyParser &src);
	ChunkedBodyParser &operator=(const ChunkedBodyParser &src);
	~ChunkedBodyParser();

	void			update(const char *data, size_t len);
	IPollElement 	*getPollElement();
	int 			getFd() const;
	short 			getEvents() const;
	int 			update(struct pollfd &pollfd, Configuration &config);
	

private:
	void		_writeFile(struct pollfd &pollfd);
	void		_readFile();
	int 		_readChunkSize();
	int 		_readChunk();
	int 		_readChunkEnd();
	void		_invalidate();

	int			_fd;
	State		_state;
	size_t		_chunk_size;
	std::string _buffer;
	std::string _file_buffer;
	std::string _filename;
};




#endif