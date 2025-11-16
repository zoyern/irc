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
#include <sstream>
#include <Sockell/SkllServer.hpp>
#include <iostream>
#include <sstream>

SkllServer::SkllServer(int max_clients, int reserved_fd)
    : _clients_max(max_clients), _reserved_fd(reserved_fd), _running(false)
{
    update_fd_limits();
}

SkllServer::~SkllServer() {}

// ═══════════════════════════════════════════════════
// FIX : Crée objet vide puis configure
// ═══════════════════════════════════════════════════
SkllNetwork& SkllServer::network(const std::string& name, int timeout, int queue) {
    _networks_owned[name] = SkllNetwork(name, timeout, queue);
    _networks_owned[name].set_server(this);
    update_fd_limits();
    return _networks_owned[name];
}

SkllChannel& SkllServer::channel(const std::string& name, bool is_default) {
    (void)is_default;
    _channels_owned[name] = SkllChannel(name);
    return _channels_owned[name];
}

int SkllServer::run() {
    _running = true;
    
    for (std::map<std::string, SkllNetwork>::iterator it = _networks_owned.begin();
         it != _networks_owned.end(); ++it) {
        it->second.run();
    }
    
    while (_running) {
        for (std::map<std::string, SkllNetwork>::iterator it = _networks_owned.begin();
             it != _networks_owned.end(); ++it) {
            it->second.update();
        }
    }
    
    for (std::map<std::string, SkllNetwork>::iterator it = _networks_owned.begin();
         it != _networks_owned.end(); ++it) {
        it->second.hook.trigger(ON_SHUTDOWN);
    }
    
    return 0;
}

void SkllServer::stop() {
    _running = false;
}

std::string SkllServer::print_networks() const {
    std::ostringstream oss;
    
    for (std::map<std::string, SkllNetwork>::const_iterator net = _networks_owned.begin();
         net != _networks_owned.end(); ++net) {
        oss << "\nNetwork: " << net->second.get_name() << std::endl;
        oss << "  Protocols: " << net->second.count_protocols() << std::endl;
    }
    
    return oss.str();
}

void SkllServer::update_fd_limits() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) != 0) return;
    
    int needed = 3 + _networks_owned.size() + _count_listening_sockets() + _clients_max + _reserved_fd;
    
    if (static_cast<int>(rl.rlim_cur) < needed) {
        rl.rlim_cur = (needed > static_cast<int>(rl.rlim_max)) ? rl.rlim_max : needed;
        setrlimit(RLIMIT_NOFILE, &rl);
    }
}

int SkllServer::_count_listening_sockets() const {
    int count = 0;
    for (std::map<std::string, SkllNetwork>::const_iterator it = _networks_owned.begin();
         it != _networks_owned.end(); ++it) {
        count += it->second.count_protocols();
    }
    return count;
}