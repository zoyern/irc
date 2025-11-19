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
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllHook.hpp>
#include <sys/resource.h>
#include <map>
#include <string>

#define SKLL_FD_MARGE 100

class SkllServer {
	private:
		int		_max_clients; // client total sur le serveur
		int		_reserved; // reserved de fd si on demande limit de fd de base 0
		int		_used; // fd utiliser actuellement permet la mise a jour equivalent a count du nombre de fd ouvert network client ou socket
		int		_count; // fd utiliser par des client ne prend pas en compte les network et socket
		bool	_running; // obligatoire mdr

		SkllChannel							*_chan_default;
		std::map<std::string, SkllChannel*>	_chans;
		std::map<std::string, SkllClient*>	_clients; // tous les client existant sur chaque network protocol
		std::map<std::string, SkllNetwork*>	_nets;
		std::map<int, SkllNetwork*>			_nets_fd;
		std::map<int, SkllClient*>			_clients_tcp;

		SkllHook							_hook;
	public:

		~SkllServer();
		SkllServer(int max_client = 1000, int reserved = 0);
		
		int		run();
		int		start();
		int		stop();

		size_t		update_fd_limits();
		SkllServer	&max_client(int size);
		SkllServer	&set_reserved_fd(int size);
		SkllServer	&on(int event, SkllHook::Callback callback , void *data);
		SkllChannel	&channels(const SkllChannel &chan, bool set_default = false); // si chan name existe deja renvoie l'objet deja existant si default true prend celui deja existant et le met en default
		SkllClient	&clients(const SkllClient &client);
		SkllNetwork	&networks(const SkllNetwork &net);	

		int			max_client() const;
		int			reserved_fd() const;
		int			used_fd() const;
		int			count() const;
		bool		running() const;
		SkllChannel	*get_channel_default();
		SkllChannel	*get_channel(const std::string &name);
		SkllClient	*get_client(const std::string &id); // adress:port
		SkllClient	*get_client(const int &fd);
		SkllNetwork	*get_network(const std::string &name);
		SkllNetwork	*get_network(const int &fd);
	private:
		SkllServer(const SkllServer &other);
		SkllServer	&operator=(const SkllServer &);


};
