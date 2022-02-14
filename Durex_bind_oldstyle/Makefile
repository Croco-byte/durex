# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: user42 <user42@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/08 14:18:25 by user42            #+#    #+#              #
#    Updated: 2022/02/14 12:00:10 by user42           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = durex

CC = gcc

CFLAGS = -Wall -Werror -Wextra -I includes/
# -fsanitize=address -g3 -fno-omit-frame-pointer 

HEADER = durex.h

MAIN = durex
CMD = cmd
DUREX_BIN = durex_bin
DUREX_DAEMON = durex_daemon durex_shell server utilities

SRCS =	$(addsuffix .c, $(addprefix srcs/, $(MAIN))) \
		$(addsuffix .c, $(addprefix srcs/cmd/, $(CMD))) \
		$(addsuffix .c, $(addprefix srcs/durex_bin/, $(DUREX_BIN))) \
		$(addsuffix .c, $(addprefix srcs/durex_daemon/, $(DUREX_DAEMON)))


OBJS = ${SRCS:.c=.o}

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lcrypt

%.o:%.c
				${CC} ${CFLAGS} -c $< -o $@

clean:
				rm -f $(OBJS)

fclean:
				rm -f $(OBJS)
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re
