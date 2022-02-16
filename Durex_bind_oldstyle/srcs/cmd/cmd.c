/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 13:07:40 by user42            #+#    #+#             */
/*   Updated: 2022/02/15 14:37:43 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "durex.h"

void	clear_env(void)
{
	char	*pathbuf;
	size_t	n;

	if (clearenv() != 0)
		exit(1);

	n = confstr(_CS_PATH, NULL, 0);
	if (n == 0)
		exit(1);
	
	if ((pathbuf = malloc(n)) == NULL)
		exit(1);

	if (confstr(_CS_PATH, pathbuf, n) == 0)
		exit(1);
	
	if (setenv("PATH", "/bin:/usr/bin:/usr/sbin", 1) == -1)
		exit(1);
	free(pathbuf);
}

int		exec_safe(char *const cmd, char **const args)
{
	pid_t	pid, ret;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		printf("[X] Error forking in exec_safe.\n");
		exit(1);
	}
	else if (pid == 0)
	{
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		if (execvp(cmd, args) == -1)
			exit(127);
	}
	else
	{
		ret = waitpid(pid, &status, 0);
		if (ret == 0 || (!WIFEXITED(status) && !WEXITSTATUS(status)))
		{
//			printf("[X] Unexpected child exit, something went wrong in exec_safe.\n");
			return(1);
		}
		if (WEXITSTATUS(status) != 0)
		{
//			printf("[X] Child status exited with an exit code different from 0 in exec_safe.\n");
			return(1);
		}
	}
	return (0);
}

/* Limited to CMD_OUT_MAX when reading output of command */
int		exec_safe_w_output(char *cmd, char **const args, char *cmd_outbuf)
{
	int		link[2];
	pid_t	pid, ret;
	int		status;

	if (pipe(link) == -1)
	{
		printf("[X] Error piping in exec_safe_w_output.\n");
		exit(1);
	}

	pid = fork();
	if (pid == -1)
	{
		printf("[X] Error forking in exec_safe_w_output.\n");
		exit(1);
	}
	else if (pid == 0)
	{
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		if (execvp(cmd, args) == -1)
			exit(127);
	}
	else
	{
		ret = waitpid(pid, &status, 0);
		if (ret == 0 || (!WIFEXITED(status) && !WEXITSTATUS(status)))
		{
//			printf("[X] Unexpected child exit, something went wrong in exec_safe_w_output.\n");
			return(1);
		}
		if (WEXITSTATUS(status) != 0)
		{
//			printf("[X] Child status exited with an exit code different from 0 in exec_safe_w_output.\n");
			return(1);
		}
		
		close(link[1]);
		int nbytes = read(link[0], cmd_outbuf, CMD_OUT_MAX);
		if (nbytes <= 0)
		{
//			printf("[X] No output from command in exec_safe_w_output.\n");
			return(1);
		}
		cmd_outbuf[nbytes - 1] = '\0';
		close(link[0]);
	}
	return (0);
}
