/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/13 13:30:41 by user42            #+#    #+#             */
/*   Updated: 2022/02/16 10:58:35 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	fatal(t_server *server)
{
	printf("[X] Server fatal error.\n");
	close(server->listen_sd);
	exit(1);
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

void	send_info(t_server *server, int fd, char *info)
{
	if ((send(fd, info, strlen(info), 0)) < 0)
		fatal(server);
}

void	send_info_all(t_server *server, char *info)
{
	t_client *tmp = server->clients;

	while (tmp)
	{
		if ((send(tmp->fd, info, strlen(info), 0)) < 0)
			fatal(server);
		tmp = tmp->next;
	}
}
