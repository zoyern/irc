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

SkllServer::SkllServer(int max_cli, int res)
    : _max_clients(max_cli), _reserved(res), _running(false)
{
    SkllSignals::setup();
    update_fd_limits();
}

SkllServer::SkllServer(const SkllServer& other)
    : _max_clients(other._max_clients), _reserved(other._reserved),
      _running(false), _nets(other._nets), _chans(other._chans)
{
    SkllSignals::setup();
    update_fd_limits();
}

SkllServer& SkllServer::operator=(const SkllServer& other) {
    if (this != &other) {
        _max_clients = other._max_clients;
        _reserved = other._reserved;
        _running = false;
        _nets = other._nets;
        _chans = other._chans;
        update_fd_limits();
    }
    return *this;
}

SkllServer::~SkllServer() {
    SkllSignals::cleanup();
}

void SkllServer::add_network(const std::string& n, SkllNetwork* net) {
    if (!net) throw SkllException("NULL network");
    _nets[n] = net;
    net->set_server(this);
    update_fd_limits();
}

void SkllServer::add_channel(const std::string& n, SkllChannel* ch) {
    if (!ch) throw SkllException("NULL channel");
    _chans[n] = ch;
}

SkllNetwork* SkllServer::get_network(const std::string& n) {
    std::map<std::string, SkllNetwork*>::iterator it = _nets.find(n);
    return (it != _nets.end()) ? it->second : NULL;
}

SkllChannel* SkllServer::get_channel(const std::string& n) {
    std::map<std::string, SkllChannel*>::iterator it = _chans.find(n);
    return (it != _chans.end()) ? it->second : NULL;
}

SkllServer& SkllServer::on(int event, SkllHook::Callback cb, void* user_data) {
    hook.on(event, cb, user_data);
    return *this;
}

int SkllServer::run() {
    _running = true;
    
    for (std::map<std::string, SkllNetwork*>::iterator it = _nets.begin(); it != _nets.end(); ++it)
        it->second->run();
    
    for (std::map<std::string, SkllNetwork*>::iterator it = _nets.begin(); it != _nets.end(); ++it)
        it->second->add_event(SkllSignals::get_read_fd());
    
    std::cout << "\n===== Sockell Server =====\n";
    for (std::map<std::string, SkllNetwork*>::iterator it = _nets.begin(); it != _nets.end(); ++it) {
        std::cout << "Network: " << it->second->get_name() << "\n";
        std::cout << it->second->print_protocols();
    }
    std::cout << "Max clients: " << _max_clients << "\n";
    std::cout << "==========================\n";
    std::cout << "✓ Started (Ctrl+C to stop)\n\n";
    
    while (_running && !SkllSignals::check()) {
        for (std::map<std::string, SkllNetwork*>::iterator it = _nets.begin(); it != _nets.end(); ++it)
            it->second->update();
    }
    
    std::cout << "\n✓ Server stopped\n";
    
    for (std::map<std::string, SkllNetwork*>::iterator it = _nets.begin(); it != _nets.end(); ++it)
        it->second->hook.trigger(ON_SHUTDOWN, NULL);
    
    return 0;
}

void SkllServer::stop() {
    _running = false;
}

void SkllServer::broadcast(const char* d, size_t l) {
    for (std::map<std::string, SkllNetwork*>::iterator it = _nets.begin(); it != _nets.end(); ++it)
        it->second->broadcast(d, l);
}

void SkllServer::update_fd_limits() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) != 0) return;
    
    int need = 3 + _nets.size() + 2 + _count_sockets() + _max_clients + _reserved;
    
    if (static_cast<int>(rl.rlim_cur) < need) {
        rl.rlim_cur = (need > static_cast<int>(rl.rlim_max)) ? rl.rlim_max : need;
        setrlimit(RLIMIT_NOFILE, &rl);
    }
}

int SkllServer::_count_sockets() const {
    int c = 0;
    for (std::map<std::string, SkllNetwork*>::const_iterator it = _nets.begin(); it != _nets.end(); ++it)
        c += it->second->protocol_count();
    return c;
}