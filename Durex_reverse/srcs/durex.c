/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 14:20:11 by user42            #+#    #+#             */
/*   Updated: 2022/02/15 11:11:29 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

int		durex_type(void)
{
	pid_t	ppid;

	ppid = getppid();
	if (ppid == 1)
		return (DUREX_DAEMON);
	return (DUREX_BIN);
}


int		main(void)
{
	if (geteuid() != 0)
	{
		printf("[X] This program must be run as root.\n");
		exit(1);
	}

	if (durex_type() == DUREX_BIN)
		durex_bin();
	else
		durex_daemon();

	return (0);
}

























/*
int		main(void)
{
	int				n = 0;
	int				fd;
	int				out_fd;
	char			buff[256];
	struct stat		st;
	void			*p;

	n = readlink("/proc/self/exe", buff, 256);
	buff[n] = '\0';
	
	fd = open(buff, O_RDONLY);
	if (fd < 0)
	{
		printf("[x] Binary errored while trying to open itself\n");
		return (1);
	}

	if (fstat(fd, &st) != 0)
	{
		printf("[x] Error while calling fstat\n");
		return (1);
	}

	p = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	out_fd = open("/home/user42/Projects/durex/copy", O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
	if (out_fd < 0)
	{
		printf("[x] Can't open the copy\n");
		return (1);
	}
	write(out_fd, p, st.st_size);

	close(fd);
	close(out_fd);


	printf("Hello World!\n");
	return (0);
}
*/
