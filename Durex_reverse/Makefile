# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: user42 <user42@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/08 14:18:25 by user42            #+#    #+#              #
#    Updated: 2022/02/11 15:07:28 by user42           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = durex

CC = gcc

CFLAGS = -Wall -Werror -Wextra -I includes/
# -fsanitize=address -g3 -fno-omit-frame-pointer 

HEADER = durex.h

MAIN = durex durex_bin durex_daemon cmd

SRCS =	$(addsuffix .c, $(addprefix srcs/, $(MAIN)))


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
