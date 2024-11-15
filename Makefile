# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bwisniew <bwisniew@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/14 13:24:31 by lcottet           #+#    #+#              #
#    Updated: 2024/11/15 17:02:47 by bwisniew         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD -MP -g3

SRCS_DIR = srcs

SRCS =	main.cpp Configuration.cpp Server.cpp Location.cpp Bind.cpp Client.cpp signal.cpp HttpRequest.cpp \
		HttpMessage.cpp InheritedParameters.cpp 


CONFIG_SRCS = Parser.cpp
SRCS += $(CONFIG_SRCS:%.cpp=config/%.cpp)

PARSING_SRCS =	Attribute.cpp Lexer.cpp Token.cpp AAttributeParser.cpp
SRCS += $(PARSING_SRCS:%.cpp=config/parsing/%.cpp) 

HTTPRESPONSE_SRCS = AHttpResponse.cpp StatusHttpResponse.cpp FileHttpResponse.cpp
SRCS += $(HTTPRESPONSE_SRCS:%.cpp=HttpResponse/%.cpp)


INCLUDE = includes includes/config includes/config/parsing includes/config/parsing/Token includes/HttpResponse

OUTDIR = obj

OBJ = $(SRCS:%.cpp=$(OUTDIR)/%.o)

DEP = $(OBJ:%.o=%.d)

NAME = webserv


all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

$(OUTDIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE:%=-I %) -o $@ -c $<

clean:
	rm -rf $(OUTDIR)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

valgrind: $(NAME)
	valgrind --exit-on-first-error=yes --error-exitcode=42 --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=valgrind.log ./$(NAME)

-include $(DEP)

.PHONY: all clean fclean re valgrind
