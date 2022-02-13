/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_shell.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/13 13:39:33 by user42            #+#    #+#             */
/*   Updated: 2022/02/13 15:15:12 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	durex_shell(t_server *server)
{
	server->server_mode = SHELL;
	reset_server(server);

	t_client			*tmp;
	struct timeval		timeout;
	char				*const args[] = {"/bin/sh", 0};

	while (1)
	{
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		tmp = server->clients;
		while (tmp)
		{
			if (tmp->pid != -1)
			{
				if ((waitpid(tmp->pid, NULL, WNOHANG)) > 0)
				{
					printf("[SHELL MODE] Client with id %d, fd %d, pid %d left\n", tmp->id, tmp->fd, tmp->pid);
					send_info(server, tmp->fd, "Bye!");
					shutdown_connection(server, tmp);
					break ;
				}
			}
			tmp = tmp->next;
		}

		if (server->shell_started != 0 && server->client_nb == 0)
		{
			printf("[SHELL MODE] No shell client left.\n");
			break ;
		}

		server->read_set = server->write_set = server->master_set;
		if (select(server->max_sd + 1, &(server->read_set), &(server->write_set), 0, &timeout) <= 0)
			continue ;
		for (int i = 0; i <= server->max_sd; i++)
		{
			if (FD_ISSET(i, &(server->read_set)))
			{
				if (i == server->listen_sd)
				{
					int clientid = add_shell_client(server);
					if (clientid == -1)
						break ;
					t_client *new_client = get_client_from_id(server, clientid);
					pid_t pid = fork();
					if (pid == 0)
					{
						dup2(new_client->fd, 0);
						dup2(new_client->fd, 1);
						dup2(new_client->fd, 2);
						execve("/bin/sh", args, NULL);
					}
					else
					{
						printf("[SHELL MODE] Client with id %d, fd %d, has pid %d\n", new_client->id, new_client->fd, pid);
						new_client->pid = pid;
						break ;
					}
				}
			}
		}
	}
	printf("[LOG] Exited shell mode, returning to daemon mode.\n");
	reset_server(server);
	return ;
}
