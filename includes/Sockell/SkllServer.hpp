/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:17:56 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:17:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllChannel.hpp>
#include <Sockell/SkllSignals.hpp>
#include <sys/resource.h>
#include <map>

class SkllServer {
	private:
		int		_max_clients;
		int		_reserved;
		bool	_running;

		std::map<std::string, SkllNetwork*>	_nets;
		std::map<std::string, SkllChannel*>	_chans;
	public:
		SkllHook	hook;

		~SkllServer();
		SkllServer(int max_cli, int res);
		SkllServer(const SkllServer &other);
		SkllServer	&operator=(const SkllServer &other);
		
		int		run();
		void	stop();
		void	add_network(const std::string &n, SkllNetwork *net);
		void	add_channel(const std::string &n, SkllChannel *ch);
		void	broadcast(const char *d, size_t l);
		void	update_fd_limits();
		
		SkllNetwork	*get_network(const std::string &n);
		SkllChannel	*get_channel(const std::string &n);
		SkllServer	&on(int event, SkllHook::Callback cb, void *user_data = NULL);

	private:
		int	_count_sockets() const;
};
