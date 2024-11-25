# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lcottet <lcottet@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/14 13:24:31 by lcottet           #+#    #+#              #
#    Updated: 2024/11/25 17:23:25 by lcottet          ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -MMD -MP -g3

SRCS_DIR = srcs

SRCS =	main.cpp Configuration.cpp Server.cpp Location.cpp Bind.cpp Client.cpp signal.cpp HttpRequest.cpp \
		HttpMessage.cpp InheritedParameters.cpp AccessLog.cpp


CONFIG_SRCS = Parser.cpp
SRCS += $(CONFIG_SRCS:%.cpp=config/%.cpp)

PARSING_SRCS =	Attribute.cpp Lexer.cpp Token.cpp AAttributeParser.cpp
SRCS += $(PARSING_SRCS:%.cpp=config/parsing/%.cpp) 

HTTPRESPONSE_SRCS = AHttpResponse.cpp StatusHttpResponse.cpp FileHttpResponse.cpp DirHttpResponse.cpp CGIHttpResponse.cpp UploadHttpResponse.cpp
SRCS += $(HTTPRESPONSE_SRCS:%.cpp=HttpResponse/%.cpp)

BODYPARSER_SRCS = BodyParser.cpp ChunkedBodyParser.cpp
SRCS += $(BODYPARSER_SRCS:%.cpp=BodyParser/%.cpp)

INCLUDE = includes includes/config includes/config/parsing includes/config/parsing/Token includes/HttpResponse includes/BodyParser

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
	valgrind --exit-on-first-error=yes --error-exitcode=42 --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all --log-file=valgrind.log ./$(NAME) tester.conf

cgi: cgi/env_cgi cgi/upper_cgi

cgi/env_cgi: cgi/env_cgi.cpp
	$(CXX) -o $@ $<

cgi/upper_cgi: cgi/upper_cgi.cpp
	$(CXX) -o $@ $<


-include $(DEP)

.PHONY: all clean fclean re valgrind cgi
