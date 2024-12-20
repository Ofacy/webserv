/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upper_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 14:33:30 by lcottet           #+#    #+#             */
/*   Updated: 2024/11/22 14:37:30 by bwisniew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

# define MIN(a,b) ((a) < (b) ? (a) : (b))

# define READ_BUFFER_SIZE 65536

int main(int argc, char *argv[], char *envp[]) {
	char buffer[READ_BUFFER_SIZE];
	std::cout << "Status: 200 OK\r\n";
	size_t ret;
	size_t content_length = 0;
	size_t i = 0;
	while (envp[i] != NULL)
	{
		if (strncmp(envp[i], "CONTENT_LENGTH=", 15) == 0)
			content_length = strtoul(envp[i] + 15, NULL, 10);
		i++;
	}
	std::cout << "Content-Length: " << content_length << "\r\n\r\n";
	size_t read_data = 0;
	while (read_data < content_length && (ret = read(0, buffer, MIN(READ_BUFFER_SIZE, content_length - read_data))) > 0)
	{
		read_data+=ret;
		for (size_t j = 0; j < ret; j++)
			buffer[j] = toupper(buffer[j]);
		std::cout << std::string(buffer, ret);
	}
	return 0;
}