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
#include <Sockell.hpp>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

class SkllSignals
{
	private:
		static int		_signal_fd;
		static sigset_t	_mask;
	public:
		static int	setup();
		static int	get_fd();
		static int	read_signal();
		static void	cleanup();
	private:
		~SkllSignals();
		SkllSignals();
		SkllSignals(const SkllSignals&);
		SkllSignals &operator=(const SkllSignals&);
};
