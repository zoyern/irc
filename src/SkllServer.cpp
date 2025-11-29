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
#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllLog.hpp>
#include <sys/resource.h>
#include <csignal>
#include <sstream>

SkllServer::SkllServer() : _running(false), _data(0) {}

SkllServer::~SkllServer() {
	/* Networks may already be destroyed if they're local objects in a containing class */
	/* Just clear the list - don't try to close them (they should close themselves) */
	_networks.clear();
}

SkllServer &SkllServer::set_fd_limit(size_t limit) {
	struct rlimit rl;
	rl.rlim_cur = limit;
	rl.rlim_max = limit;
	if (setrlimit(RLIMIT_NOFILE, &rl) == 0) {
		std::ostringstream oss;
		oss << "FD limit set to " << limit;
		SkllLog::info(oss.str());
	} else {
		SkllLog::warn("Failed to set FD limit");
	}
	return *this;
}

SkllServer &SkllServer::add(SkllNetwork &net) {
	net.set_server(this);
	_networks.push_back(&net);
	return *this;
}

size_t SkllServer::network_count() const { return _networks.size(); }
SkllChannels &SkllServer::channels() { return _channels; }

SkllServer &SkllServer::set_default_channel(const std::string &name) {
	_default_channel = name;
	return *this;
}

const std::string &SkllServer::default_channel() const { return _default_channel; }
SkllSignals &SkllServer::signals() { return _signals; }

SkllServer &SkllServer::on(int type, SkllCallback fn) {
	if (type &SKLL_EV_START)  _on_start.add(fn);
	if (type &SKLL_EV_STOP)   _on_stop.add(fn);
	if (type &SKLL_EV_SIGNAL) _on_signal.add(fn);
	return *this;
}

bool SkllServer::init() {
	if (!_signals.setup()) {
		SkllLog::error("Failed to setup signals");
		return false;
	}
	
	for (size_t i = 0; i < _networks.size(); ++i) {
		if (!_networks[i]->init()) {
			std::ostringstream oss;
			oss << "Failed to init network " << _networks[i]->name();
			SkllLog::error(oss.str());
			return false;
		}
	}
	
	SkllEvent event;
	event.set_event_type(SKLL_EV_START);
	event.set_server(this);
	_on_start.run(event);
	
	return true;
}

int SkllServer::run() {
	_running = true;
	SkllLog::success("Server running");
	
	while (_running) {
		for (size_t i = 0; i < _networks.size(); ++i)
			_networks[i]->poll_once();
		
		if (_signals.pending())
			_handle_signal();
	}
	
	SkllEvent event;
	event.set_event_type(SKLL_EV_STOP);
	event.set_server(this);
	_on_stop.run(event);
	
	SkllLog::info("Server stopped");
	return 0;
}

void SkllServer::stop() { _running = false; }
bool SkllServer::running() const { return _running; }

void SkllServer::_handle_signal() {
	while (_signals.pending()) {
		int sig = _signals.pop();
		
		if (sig == SIGPIPE) continue;
		
		std::ostringstream oss;
		oss << "Signal " << sig << " received";
		SkllLog::info(oss.str());
		
		SkllEvent event;
		event.set_event_type(SKLL_EV_SIGNAL);
		event.set_server(this);
		event.set_signal_number(sig);
		
		int ret = _on_signal.run(event);
		if (ret == SKLL_FATAL || sig == SIGINT || sig == SIGTERM)
			_running = false;
	}
}
