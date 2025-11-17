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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>

SkllProtocol::SkllProtocol() : _fd(-1), _port(0), _opts(0), _bound(false) {}

SkllProtocol::SkllProtocol(const SkllProtocol& other)
    : _fd(-1), _name(other._name), _addr(other._addr), _port(other._port),
      _opts(other._opts), _bound(false), _clients(other._clients) {}

SkllProtocol& SkllProtocol::operator=(const SkllProtocol& other) {
    if (this != &other) {
        if (_fd >= 0) close(_fd);
        _fd = -1;
        _name = other._name;
        _addr = other._addr;
        _port = other._port;
        _opts = other._opts;
        _bound = false;
        _clients = other._clients;
    }
    return *this;
}

SkllProtocol::~SkllProtocol() {
    if (_fd >= 0) close(_fd);
}

void SkllProtocol::create(const std::string& name, const std::string& addr, int port, int opts) {
    _name = name;
    _addr = addr;
    _port = port;
    _opts = opts;
    
    int family = (opts & SKLL_IPV6) ? AF_INET6 : AF_INET;
    int type = (opts & SKLL_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    
    _fd = socket(family, type, 0);
    if (_fd < 0) throw SkllErrorSocket(_err_socket());
    
    fcntl(_fd, F_SETFL, O_NONBLOCK);
    set_reuseaddr().set_nodelay().set_quickack();
}

void SkllProtocol::run() {
    if (_bound) return;
    
    int family = (_opts & SKLL_IPV6) ? AF_INET6 : AF_INET;
    
    if (family == AF_INET) {
        struct sockaddr_in sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(_port);
        inet_pton(AF_INET, _addr.c_str(), &sa.sin_addr);
        if (bind(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0)
            throw SkllErrorBind(_err_bind());
    } else {
        struct sockaddr_in6 sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sin6_family = AF_INET6;
        sa.sin6_port = htons(_port);
        inet_pton(AF_INET6, _addr.c_str(), &sa.sin6_addr);
        if (bind(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0)
            throw SkllErrorBind(_err_bind());
    }
    
    if ((_opts & SKLL_UDP) == 0)
        if (listen(_fd, 128) < 0) throw SkllErrorListen(_err_listen());
    
    _bound = true;
}

void SkllProtocol::add_client(int fd, SkllClient* c) { _clients[fd] = c; }
void SkllProtocol::remove_client(int fd) { _clients.erase(fd); }

SkllClient* SkllProtocol::get_client(int fd) {
    std::map<int, SkllClient*>::iterator it = _clients.find(fd);
    return (it != _clients.end()) ? it->second : NULL;
}

const std::map<int, SkllClient*>& SkllProtocol::get_clients() const {
    return _clients;
}

void SkllProtocol::send(int fd, const char* data, size_t len) {
    ::send(fd, data, len, 0);
}

void SkllProtocol::broadcast(const char* data, size_t len) {
    for (std::map<int, SkllClient*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        ::send(it->first, data, len, 0);
}

SkllProtocol& SkllProtocol::set_reuseaddr(bool e) {
    if (_fd < 0) return *this;
    int o = e ? 1 : 0;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &o, sizeof(o));
    return *this;
}

SkllProtocol& SkllProtocol::set_nodelay(bool e) {
    if (_fd < 0 || !is_tcp()) return *this;
    int o = e ? 1 : 0;
    setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &o, sizeof(o));
    return *this;
}

SkllProtocol& SkllProtocol::set_quickack(bool e) {
    if (_fd < 0 || !is_tcp()) return *this;
    int o = e ? 1 : 0;
    setsockopt(_fd, IPPROTO_TCP, TCP_QUICKACK, &o, sizeof(o));
    return *this;
}

SkllProtocol& SkllProtocol::set_keepalive(bool e) {
    if (_fd < 0) return *this;
    int o = e ? 1 : 0;
    setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &o, sizeof(o));
    return *this;
}

SkllProtocol& SkllProtocol::on(int event, SkllHook::Callback cb, void* user_data) {
    hook.on(event, cb, user_data);
    return *this;
}

int SkllProtocol::get_fd() const { return _fd; }
std::string SkllProtocol::get_name() const { return _name; }
std::string SkllProtocol::get_address() const { return _addr; }
int SkllProtocol::get_port() const { return _port; }
bool SkllProtocol::is_tcp() const { return (_opts & SKLL_TCP) != 0; }
bool SkllProtocol::is_bound() const { return _bound; }

std::string SkllProtocol::_err_socket() {
    std::ostringstream o;
    o << "socket(): ";
    switch (errno) {
        case EACCES: o << "Permission denied"; break;
        case EMFILE: o << "Process FD limit"; break;
        case ENFILE: o << "System FD limit"; break;
        default: o << std::strerror(errno);
    }
    return o.str();
}

std::string SkllProtocol::_err_bind() {
    std::ostringstream o;
    o << "bind(" << _addr << ":" << _port << "): ";
    switch (errno) {
        case EADDRINUSE: o << "Address in use"; break;
        case EACCES: o << "Permission denied"; break;
        default: o << std::strerror(errno);
    }
    return o.str();
}

std::string SkllProtocol::_err_listen() {
    std::ostringstream o;
    o << "listen(): " << std::strerror(errno);
    return o.str();
}