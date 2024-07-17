# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mrubina <mrubina@student.42heilbronn.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/12 18:35:29 by mrubina           #+#    #+#              #
#    Updated: 2024/07/10 16:59:11 by mrubina          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS = -std=c++98 
# -Wall -Werror -Wextra

SRC = main.cpp IrcServ.cpp Err.cpp User.cpp Channel.cpp Command.cpp Message.cpp Mode.cpp
OBJ = $(SRC:.cpp=.o)
NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ)
	c++ -o $(NAME) $(OBJ) $(CXXFLAGS)

%.cpp.o: %.cpp
	c++ $(CXXFLAGS) -c  $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all