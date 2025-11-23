/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:09:24 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:09:24 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllChannel.hpp>
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllHook.hpp>
#include <sys/resource.h>
#include <map>
#include <string>
#include <iostream>

#define SKLLSERVER_FDLOCKED	1024

class SkllServer
{
	private:
		bool	_started;
		bool	_running;
		bool	_stopped;
		bool	_mute;
		size_t	_max_clients;
		size_t	_reserved;
		size_t	_used;
		size_t	_fd_limit;
		
		SkllHook							_hook;
		SkllChannel							*_chan_default;
		std::map<std::string, SkllChannel*>	_chans;
		std::map<std::string, SkllClient*>	_clients;
		std::map<std::string, SkllNetwork*>	_nets;
		std::map<int, SkllNetwork*>			_nets_fd;
		std::map<int, SkllClient*>			_clients_tcp;

	public:
		~SkllServer();
		SkllServer(int max_client = 1000, bool mute = false, int reserved = 0);
		
		int		start();
		int		run();
		int		stop();
		int		shutdown();
		void	broadcast(const std::string &msg);
		void	broadcast(const std::string &msg, int opts);
		void	broadcast(SkllMessage &msg);
		size_t	update_fd_limits();
		
	private:
		SkllServer(const SkllServer&);
		SkllServer 	&operator=(const SkllServer&);
		void		trigger_event(int type);

	public:
		SkllServer	&max_client(size_t size);
		SkllServer	&reserved_fd(size_t size);
		SkllServer	&mute(bool enable);
		SkllServer	&on(int event, SkllHook::Callback callback, void *data);
		SkllChannel	&channels(const SkllChannel &chan, bool set_default = false);
		SkllClient	&clients(const SkllClient &client);
		SkllNetwork	&networks(const SkllNetwork &net);
		
		bool		is_started() const;
		bool		is_running() const;
		bool		is_stopped() const;
		bool		is_muted() const;
		size_t		count() const;
		size_t		used_fd() const;
		size_t		reserved_fd() const;
		size_t		get_fd_limit() const;
		size_t		total_networks() const;
		size_t		total_clients() const;
		SkllChannel	*get_channel_default();
		SkllChannel	*get_channel(const std::string &name);
		SkllClient	*get_client(const std::string &id);
		SkllClient	*get_client(int fd);
		SkllNetwork	*get_network(const std::string &name);
		SkllNetwork	*get_network(int fd);

		typedef std::map<std::string, SkllChannel*>::iterator   ChannelsMapIt;
		typedef std::map<std::string, SkllClient*>::iterator    ClientsMapIt;
		typedef std::map<std::string, SkllNetwork*>::iterator   NetworksMapIt;
		typedef std::map<int, SkllNetwork*>::iterator           NetworksFdMapIt;
		typedef std::map<int, SkllClient*>::iterator            ClientsFdMapIt;
};
