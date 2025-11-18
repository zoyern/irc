/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 03:28:33 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 03:28:33 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllServer.hpp>
#include <iostream>

SkllServer::~SkllServer() {}
SkllServer::SkllServer(int max_cli, int res)
    : _max_clients(max_cli)
	, _reserved(res)
	, _used(3)
	, _count(0)
	, _running(false)
	, _chan_default(NULL)
{}

int	SkllServer::run() {
	typedef std::map<std::string, SkllNetwork*>::iterator It;
	_running = true;
	start();
	std::cout << "✓ Started (Ctrl+C to stop)" << std::endl;
	std::cout << "[Server] Starting listen on : " << _nets.size() << " Network(s)" << std::endl;
	while (_running) {
        for (It it = _nets.begin(); it != _nets.end(); ++it)
            it->second->wait();
		hook.trigger(ON_UPDATE, NULL);
    }
	hook.trigger(ON_SHUTDOWN, NULL);
	for (It it = _nets.begin(); it != _nets.end(); ++it)
        it->second->shutdown();
	return (0);
}

int SkllServer::start() {
	typedef std::map<std::string, SkllNetwork*>::iterator It;
	std::cout << "[Server] Initialisation..." << std::endl;
	for (It it = _nets.begin(); it != _nets.end(); ++it)
        it->second->start();
	hook.trigger(ON_START, NULL);
	return (0);
}

int SkllServer::stop() { _running = false; return (0);}

size_t SkllServer::update_fd_limits() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) != 0) throw (std::runtime_error("getrlimit failed")); // géré les erreur si pas recuperer throw ? 
    size_t needed = _used + _reserved + SKLL_FD_MARGE;
    rl.rlim_cur = needed <= rl.rlim_max  ? needed : throw (std::runtime_error("you reach the maximum fd available"));
    if (setrlimit(RLIMIT_NOFILE, &rl) != 0) throw (std::runtime_error("setrlimit failed"));
    return (rl.rlim_cur);
}

SkllServer	&SkllServer::max_client(int size) { _max_clients = size; update_fd_limits(); return (*this);}
SkllServer	&SkllServer::set_reserved_fd(int size) { _reserved = size; update_fd_limits(); return (*this);}
SkllServer	&SkllServer::on(int event, SkllHook::Callback callback, void* data) { hook.on(event, callback, data); return (*this); }
SkllChannel	&SkllServer::channels(const SkllChannel &chan, bool set_default)
{
	typedef std::map<std::string, SkllChannel*>::iterator It;
	std::pair<It, bool> res = _chans.insert(std::make_pair(chan.name, (SkllChannel*)&chan));
	if (set_default)
		_chan_default = res.first->second;
	return (*(res.first->second));
}
SkllClient	&SkllServer::clients(const SkllClient &client)
{
	typedef std::map<std::string, SkllClient*>::iterator It;
	std::pair<It, bool> res = _clients.insert(std::make_pair(client.id, (SkllClient*)&client));
    if (client.fd >= 0) {
        _clients_tcp[client.fd] = res.first->second; }
	return (*(res.first->second));
}

SkllNetwork		&SkllServer::networks(const SkllNetwork &net)
{
	typedef std::map<std::string, SkllNetwork*>::iterator It;
    std::pair<It, bool> res = _nets.insert(std::make_pair(net.name(), (SkllNetwork*)&net));
    _nets_fd[net.fd()] = res.first->second;
    return (*(res.first->second));
}
int	SkllServer::max_client() const { return _max_clients; }
int	SkllServer::reserved_fd() const { return _reserved; }
int SkllServer::used_fd() const { return _used; }
int SkllServer::count() const { return _count; }
bool SkllServer::running() const { return _running; }

SkllChannel	*SkllServer::get_channel_default() { return (_chan_default); }
SkllChannel	*SkllServer::get_channel(const std::string &name) { std::map<std::string, SkllChannel*>::iterator it = _chans.find(name); return ((it != _chans.end()) ? it->second : NULL);}
SkllClient	*SkllServer::get_client(const std::string &id) { std::map<std::string, SkllClient*>::iterator it = _clients.find(id); return ((it != _clients.end()) ? it->second : NULL); }
SkllClient	*SkllServer::get_client(const int &fd) { std::map<int, SkllClient*>::iterator it = _clients_tcp.find(fd); return ((it != _clients_tcp.end()) ? it->second : NULL); }
SkllNetwork	*SkllServer::get_network(const std::string &name) { std::map<std::string, SkllNetwork*>::iterator it = _nets.find(name); return ((it != _nets.end()) ? it->second : NULL); }
SkllNetwork	*SkllServer::get_network(const int &fd) { std::map<int, SkllNetwork*>::iterator it = _nets_fd.find(fd); return ((it != _nets_fd.end()) ? it->second : NULL); }

SkllServer::SkllServer(const SkllServer &) {}
SkllServer	&SkllServer::operator=(const SkllServer &) { return (*this); }