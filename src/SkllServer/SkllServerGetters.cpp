/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServerGetters.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:17:37 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:17:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllServer.hpp>

bool		SkllServer::is_started() const { return (_started); }
bool		SkllServer::is_running() const { return (_running); }
bool		SkllServer::is_stopped() const { return (_stopped); }
bool		SkllServer::is_muted() const { return (_mute); }
size_t		SkllServer::count() const { return (_max_clients); }
size_t		SkllServer::used_fd() const { return (_used); }
size_t		SkllServer::reserved_fd() const { return (_reserved); }
size_t		SkllServer::get_fd_limit() const { return (_fd_limit); }
size_t		SkllServer::total_networks() const { return (_nets.size()); }
size_t		SkllServer::total_clients() const
{
	size_t total = 0;
	for (std::map<std::string, SkllNetwork*>::const_iterator it = _nets.begin();
		 it != _nets.end(); ++it) {
		const std::map<std::string, SkllProtocol*> &protocols = it->second->get_protocols();
		for (std::map<std::string, SkllProtocol*>::const_iterator pit = protocols.begin(); pit != protocols.end(); ++pit)
			total += pit->second->get_clients().size();
	}
	return (total);
}

SkllChannel	*SkllServer::get_channel_default() { return (_chan_default); }
SkllChannel	*SkllServer::get_channel(const std::string &name) { ChannelsMapIt it = _chans.find(name); return ((it != _chans.end()) ? it->second : NULL); }
SkllClient	*SkllServer::get_client(const std::string &id) { ClientsMapIt it = _clients.find(id); return ((it != _clients.end()) ? it->second : NULL); }
SkllClient	*SkllServer::get_client(int fd) { ClientsFdMapIt it = _clients_tcp.find(fd); return ((it != _clients_tcp.end()) ? it->second : NULL); }
SkllNetwork	*SkllServer::get_network(const std::string &name) { NetworksMapIt it = _nets.find(name); return ((it != _nets.end()) ? it->second : NULL); }
SkllNetwork	*SkllServer::get_network(int fd) { NetworksFdMapIt it = _nets_fd.find(fd); return ((it != _nets_fd.end()) ? it->second : NULL); }



