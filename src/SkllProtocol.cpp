/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllProtocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 17:04:16 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 17:04:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllErrors.hpp>
#include <Sockell.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

SkllProtocolOptions::SkllProtocolOptions()
    : reuse_addr(true), reuse_port(false)
    , send_buf_size(0), recv_buf_size(0)
    , send_timeout(0), recv_timeout(0)
    , buffer_size(8192), chunk_size(4096) {}

SkllProtocolTCPOptions::SkllProtocolTCPOptions()
    : nodelay(true), cork(false), quickack(false), keepalive(true)
    , keepalive_time(7200), keepalive_intvl(75), keepalive_probes(9) {}

SkllProtocol::SkllProtocol()
    : _fd(-1), _name(""), _addr(""), _port(0), _opts(0)
    , _started(false), _running(false), _stopped(false)
    , _network(NULL) {}

SkllProtocol::SkllProtocol(const std::string &name, const std::string &addr, int port, int opts)
    : _fd(-1), _name(name), _addr(addr), _port(port), _opts(opts)
    , _started(false), _running(false), _stopped(false)
    , _network(NULL) {}

SkllProtocol::~SkllProtocol() {
    shutdown();
}

int SkllProtocol::start() {
	if (_started) return 0;
	int family = (_opts  &SKLL_IPV6) ? AF_INET6 : AF_INET;
	int type = (_opts  &SKLL_UDP) ? SOCK_DGRAM : SOCK_STREAM;
	_fd = socket(family, type, 0);
	if (_fd < 0) throw SkllErrorSocket("socket() failed");
	apply_socket_options();
	_started = true;
	trigger_event(SKLL_ON_START);
	return 0;
}

int SkllProtocol::run() {
	if (!_started) start();
	if (_running) return 0;
	bind_and_listen();
	_running = true;
	return 0;
}

int SkllProtocol::stop() {
    if (!_running || _stopped) return 0;
    
    _stopped = true;
    _running = false;
    
    trigger_event(SKLL_ON_STOP);
    
    return 0;
}

int SkllProtocol::shutdown() {
    if (_fd < 0) return 0;
    
    _clients.clear();
    
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
    
    _network = NULL;
    _started = false;
    _running = false;
    _stopped = true;
    
    trigger_event(SKLL_ON_SHUTDOWN);
    
    return 0;
}

void SkllProtocol::bind_and_listen() {
    if (_opts  &SKLL_IPV6) {
        sockaddr_in6 addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(_port);
        if (_addr == "0.0.0.0" || _addr.empty()) {
            addr.sin6_addr = in6addr_any;
        } else {
            inet_pton(AF_INET6, _addr.c_str(), &addr.sin6_addr);
        }
        
        if (bind(_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            throw SkllErrorBind("bind() failed");
        }
    } else {
        sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_port);
        if (_addr == "0.0.0.0" || _addr.empty()) {
            addr.sin_addr.s_addr = INADDR_ANY;
        } else {
            inet_pton(AF_INET, _addr.c_str(), &addr.sin_addr);
        }
        
        if (bind(_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            throw SkllErrorBind("bind() failed");
        }
    }
    
    if (_opts  &SKLL_TCP) {
        if (listen(_fd, 128) < 0) {
            throw SkllErrorListen("listen() failed");
        }
    }
    
    fcntl(_fd, F_SETFL, O_NONBLOCK);
}

void SkllProtocol::apply_socket_options() {
	if (_fd < 0) return;
	
	int val = _options.reuse_addr ? 1 : 0;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	
	val = _options.reuse_port ? 1 : 0;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
	
	if (_options.send_buf_size > 0) {
		setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &_options.send_buf_size, sizeof(int));
	}
	
	if (_options.recv_buf_size > 0) {
		setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &_options.recv_buf_size, sizeof(int));
	}
	
	// ✅ TCP options (Linux 2.6+ - pas besoin de #ifdef)
	if (_opts  &SKLL_TCP) {
		val = _tcp_options.nodelay ? 1 : 0;
		setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
		
		val = _tcp_options.keepalive ? 1 : 0;
		setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
	}
}

SkllProtocol &SkllProtocol::set_reuseaddr(bool enable) {
    if (_running) return *this;
    _options.reuse_addr = enable;
    return *this;
}

SkllProtocol &SkllProtocol::set_nodelay(bool enable) {
    if (_running) return *this;
    _tcp_options.nodelay = enable;
    return *this;
}

SkllProtocol &SkllProtocol::set_keepalive(bool enable) {
    if (_running) return *this;
    _tcp_options.keepalive = enable;
    return *this;
}

SkllProtocol &SkllProtocol::set_buffer_sizes(int send_size, int recv_size) {
    if (_running) return *this;
    _options.send_buf_size = send_size;
    _options.recv_buf_size = recv_size;
    return *this;
}

SkllProtocol &SkllProtocol::set_chunk_size(int size) {
    if (_running) return *this;
    _options.chunk_size = size;
    return *this;
}

SkllProtocol &SkllProtocol::on(int event, SkllHook::Callback callback, void *user_data) {
    _hook.on(event, callback, user_data);
    return *this;
}

void SkllProtocol::trigger_event(int type, SkllClient *client, SkllMessage *msg, int error) {
    SkllEvent event;
    event.type = type;
    event.protocol = this;
    event.network = _network;
    event.server = _network ? _network->get_server() : NULL;
    event.client = client;
    event.message = msg;
    event.error_code = error;
    
    _hook.trigger(type, &event);
}

SkllRouter &SkllProtocol::router() {
    return _router;
}

void SkllProtocol::add_client(int fd, SkllClient *client) {
    _clients[fd] = client;
}

void SkllProtocol::remove_client(int fd) {
    _clients.erase(fd);
}

SkllClient *SkllProtocol::get_client(int fd) {
    std::map<int, SkllClient*>::iterator it = _clients.find(fd);
    return (it != _clients.end()) ? it->second : NULL;
}

const std::map<int, SkllClient*> &SkllProtocol::get_clients() const {
    return _clients;
}

void SkllProtocol::send(int fd, SkllMessage &msg) {
    msg.set_protocol(this);
    msg.set_fd(fd);
    msg.send_to(fd);
    trigger_event(SKLL_ON_SEND, get_client(fd), &msg);
}

void SkllProtocol::send(int fd, const std::string &data) {
    SkllMessage msg;
    msg.set_protocol(this);
    msg.set_fd(fd);
    msg << data;
    msg.send();
}

void SkllProtocol::broadcast(SkllMessage &msg) {
    for (std::map<int, SkllClient*>::iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        send(it->first, msg);
    }
}

void SkllProtocol::broadcast(const std::string &data) {
    for (std::map<int, SkllClient*>::iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        send(it->first, data);
    }
}

void SkllProtocol::on_recv_data(int fd, const uint8_t *data, size_t len) {
    SkllClient *client = get_client(fd);
    if (!client) return;
    
    SkllEvent event;
    event.type = SKLL_ON_RECV;
    event.protocol = this;
    event.network = _network;
    event.server = _network ? _network->get_server() : NULL;
    event.client = client;
    event.message = &client->recv_msg;
    event.fd = fd;
    
    _router.on_recv_data(data, len, &event);
    
    trigger_event(SKLL_ON_RECV, client, &client->recv_msg);
}

void SkllProtocol::set_network(SkllNetwork *network) {
    _network = network;
}

SkllNetwork *SkllProtocol::get_network() const {
    return _network;
}

int SkllProtocol::get_fd() const { return _fd; }
const std::string &SkllProtocol::get_name() const { return _name; }
const std::string &SkllProtocol::get_address() const { return _addr; }
int SkllProtocol::get_port() const { return _port; }
bool SkllProtocol::is_tcp() const { return (_opts & SKLL_TCP); }
bool SkllProtocol::is_udp() const { return (_opts & SKLL_UDP); }
bool SkllProtocol::is_started() const { return _started; }
bool SkllProtocol::is_running() const { return _running; }
bool SkllProtocol::is_stopped() const { return _stopped; }