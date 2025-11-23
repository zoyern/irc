/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:18:09 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:18:09 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllServer.hpp>

SkllServer::~SkllServer() { shutdown(); }
SkllServer::SkllServer(int max_cli, bool mute, int res)
	: _started(false), _running(false), _stopped(false), _mute(mute)
	, _max_clients(max_cli), _reserved(res), _used(SKLLSERVER_FDLOCKED)
	, _fd_limit(0), _chan_default(NULL) {}

int	SkllServer::start() {
	if (_started) return (0);

	update_fd_limits();
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it)
		it->second->start();
	_started = true;
	trigger_event(SKLL_ON_START);
	return (0);
}

int	SkllServer::run() {
	if (_running) return (0);
	if (!_started) start();

	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it)
		it->second->run();
	_running = true;
	while (_running) {
		for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it)
			it->second->update();
		trigger_event(SKLL_ON_UPDATE);
	}
	return (0);
}

int	SkllServer::stop() {
	if (!_running || _stopped) return (0);

	if (!_mute) std::cout << SKLL_YELLOW << "\n● " << SKLL_RESET << "Shutting down..." << std::endl;
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it)
		it->second->stop();
	_running = false;
	_stopped = true;
	trigger_event(SKLL_ON_STOP);
	return (0);
}

int SkllServer::shutdown() {
	if (!_started) return (0);

	trigger_event(SKLL_ON_SHUTDOWN);
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it)
		it->second->shutdown();
	_started = false;
	_running = false;
	_stopped = true;
	return (0);
}

void	SkllServer::broadcast(const std::string &msg) {
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it) {
		const std::map<std::string, SkllProtocol*> &protocols = it->second->get_protocols();
		for (std::map<std::string, SkllProtocol*>::const_iterator pit = protocols.begin(); pit != protocols.end(); ++pit)
			pit->second->broadcast(msg);
	}
}

void	SkllServer::broadcast(const std::string &msg, int opts) {
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it) {
		const std::map<std::string, SkllProtocol*> &protocols = it->second->get_protocols();
		for (std::map<std::string, SkllProtocol*>::const_iterator pit = protocols.begin(); pit != protocols.end(); ++pit) {
			SkllProtocol *proto = pit->second;
			if (opts & SKLL_TCP && !proto->is_tcp()) continue;
			if (opts & SKLL_UDP && !proto->is_udp()) continue;
			proto->broadcast(msg);
		}
	}
}

void	SkllServer::broadcast(SkllMessage &msg) {
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it) {
		const std::map<std::string, SkllProtocol*> &protocols = it->second->get_protocols();
		for (std::map<std::string, SkllProtocol*>::const_iterator pit = protocols.begin(); pit != protocols.end(); ++pit)
			pit->second->broadcast(msg);
	}
}

size_t	SkllServer::update_fd_limits() {
	struct rlimit rl;
	size_t needed = _max_clients + _used + _reserved;
	if (getrlimit(RLIMIT_NOFILE, &rl) != 0)
		throw std::runtime_error("getrlimit() failed");
	if (needed > rl.rlim_max)
		throw std::runtime_error("FD limit exceeded");
	rl.rlim_cur = needed;
	if (setrlimit(RLIMIT_NOFILE, &rl) != 0)
		throw std::runtime_error("setrlimit() failed");
	_fd_limit = rl.rlim_cur;
	return (_fd_limit);
}
