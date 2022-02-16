/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_shell.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/13 13:39:33 by user42            #+#    #+#             */
/*   Updated: 2022/02/16 12:26:38 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	durex_shell(t_server *server)
{
	server->server_mode = SHELL;
	reset_server(server);

	t_client			*tmp;
	char				*const args[] = {"/bin/sh", 0};
	int					ready;

	while (1)
	{
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

		ready = poll(server->pfds, 1, 0);									// Only the server listen_sd to watch for incoming connections.
		if (ready <= 0)
			continue ;

		for (int i = 0; i < 1; i++)
		{
			if (server->pfds[i].revents & POLLIN)
			{
				if (server->pfds[i].fd == server->listen_sd)				// Should always be the case, but who knows
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
						printf("[SHELL MODE] Client with id %d, fd %d, opened shell (pid %d)\n", new_client->id, new_client->fd, pid);
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
