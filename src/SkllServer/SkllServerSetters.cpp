/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServerSetters.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:17:41 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:17:41 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllServer.hpp>

SkllServer	&SkllServer::max_client(size_t size) { if (_running) return (*this); _max_clients = size; update_fd_limits(); return (*this); }
SkllServer	&SkllServer::reserved_fd(size_t size) { if (_running) return (*this); _reserved = size; update_fd_limits(); return (*this); }
SkllServer	&SkllServer::mute(bool enable) { _mute = enable; return (*this); }
SkllServer	&SkllServer::on(int event, SkllHook::Callback callback, void *data) { _hook.on(event, callback, data); return (*this); }

SkllChannel	&SkllServer::channels(const SkllChannel &chan, bool set_default) {
	std::pair<ChannelsMapIt, bool> res = _chans.insert(
		std::make_pair(chan.name, (SkllChannel*)&chan));
	if (set_default) _chan_default = res.first->second;
	return (*(res.first->second));
}

SkllClient	&SkllServer::clients(const SkllClient &client) {
	std::pair<ClientsMapIt, bool> res = _clients.insert(
		std::make_pair(client.id, (SkllClient*)&client));
	if (client.fd >= 0) _clients_tcp[client.fd] = res.first->second;
	return (*(res.first->second));
}

SkllNetwork	&SkllServer::networks(const SkllNetwork &net) {
	SkllNetwork *network = const_cast<SkllNetwork*>(&net);
	network->set_server(this);
	std::pair<NetworksMapIt, bool> res = _nets.insert(
		std::make_pair(network->get_name(), network));
	_nets_fd[network->get_epoll_fd()] = res.first->second;
	return (*(res.first->second));
}
