/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllSignals.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 02:31:16 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 02:31:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

class SkllSignals {
	private:
		static int	_pipe[2];
		static void	_handler(int sig);

	public:
		static void	setup();
		static void	cleanup();
		static int	get_read_fd();
		static bool	check();
};
