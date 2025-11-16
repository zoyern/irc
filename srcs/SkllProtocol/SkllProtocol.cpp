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

// SkllProtocol.cpp
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


SkllProtocol::SkllProtocol()
    : _fd(-1), _name(""), _address(""), _port(0), _options(0), _bound(false)
    , _buffer_size(4096), _chunk_size(4096), _crlf("\r\n")
    , _timeout_sec(0), _keepidle(0), _keepintvl(0)
{}

SkllProtocol::SkllProtocol(const std::string& name, const std::string& addr, int port, int opts)
    : _fd(-1), _name(""), _address(""), _port(0), _options(0), _bound(false)
    , _buffer_size(4096), _chunk_size(4096), _crlf("\r\n")
    , _timeout_sec(0), _keepidle(0), _keepintvl(0)
{
    create(name, addr, port, opts);
}

SkllProtocol::~SkllProtocol() {
    if (_fd >= 0) close(_fd);
}



SkllProtocol::SkllProtocol(const SkllProtocol& other) { *this = other;}

SkllProtocol& SkllProtocol::operator=(const SkllProtocol& other) {
    if (this != &other) {
        _fd = -1;
        _name = other._name;
        _address = other._address;
        _port = other._port;
        _options = other._options;
		_bound = false;
        _buffer_size = other._buffer_size;
        _chunk_size = other._chunk_size;
        _crlf = other._crlf;
        _timeout_sec = other._timeout_sec;
        _keepidle = other._keepidle;
        _keepintvl = other._keepintvl;
    }
    return (*this);
}

void SkllProtocol::create(const std::string& name, const std::string& addr, int port, int opts) {
    _name = name;
    _address = addr;
    _port = port;
    _options = opts;
    
    int family = (opts & SKLL_IPV6) ? AF_INET6 : AF_INET;
    int type = (opts & SKLL_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    
    _fd = socket(family, type, 0);
    if (_fd < 0) throw SkllErrorSocket(_get_socket_error());
    
    fcntl(_fd, F_SETFL, O_NONBLOCK);
    
    // Défauts optimisés
    set_reuseaddr(true);
    if (is_tcp()) {
        set_nodelay(true);
        set_quickack(true);
    }
}

void SkllProtocol::run() {
    if (_bound) return;
    
    int family = (_options & SKLL_IPV6) ? AF_INET6 : AF_INET;
    int type = (_options & SKLL_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    
    if (family == AF_INET) {
        struct sockaddr_in sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_port = htons(_port);
        inet_pton(AF_INET, _address.c_str(), &sa.sin_addr);
        
        if (bind(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
            throw SkllErrorBind(_get_bind_error());
        }
    } else {
        struct sockaddr_in6 sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sin6_family = AF_INET6;
        sa.sin6_port = htons(_port);
        inet_pton(AF_INET6, _address.c_str(), &sa.sin6_addr);
        
        if (bind(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
            throw SkllErrorBind(_get_bind_error());
        }
    }
    
    if (type == SOCK_STREAM) {
        if (listen(_fd, 128) < 0) {
            throw SkllErrorListen(_get_listen_error());
        }
    }
    
    _bound = true;
}

// ═══════════════════════════════════════════════════
// SETTERS
// ═══════════════════════════════════════════════════
SkllProtocol& SkllProtocol::set_reuseaddr(bool enable) {
    if (_fd < 0) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_reuseport(bool enable) {
    if (_fd < 0) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_keepalive(bool enable) {
    if (_fd < 0) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_nodelay(bool enable) {
    if (_fd < 0 || !is_tcp()) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_quickack(bool enable) {
    if (_fd < 0 || !is_tcp()) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, IPPROTO_TCP, TCP_QUICKACK, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_cork(bool enable) {
    if (_fd < 0 || !is_tcp()) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_linger(bool enable, int sec) {
    if (_fd < 0) return *this;
    struct linger lin;
    lin.l_onoff = enable ? 1 : 0;
    lin.l_linger = sec;
    setsockopt(_fd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));
    return *this;
}

SkllProtocol& SkllProtocol::set_rcvbuf(int size) {
    if (_fd < 0) return *this;
    setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    return *this;
}

SkllProtocol& SkllProtocol::set_sndbuf(int size) {
    if (_fd < 0) return *this;
    setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    return *this;
}

SkllProtocol& SkllProtocol::set_rcvlowat(int bytes) {
    if (_fd < 0) return *this;
    setsockopt(_fd, SOL_SOCKET, SO_RCVLOWAT, &bytes, sizeof(bytes));
    return *this;
}

SkllProtocol& SkllProtocol::set_sndlowat(int bytes) {
    if (_fd < 0) return *this;
    setsockopt(_fd, SOL_SOCKET, SO_SNDLOWAT, &bytes, sizeof(bytes));
    return *this;
}

SkllProtocol& SkllProtocol::set_keepidle(int sec) {
    if (_fd < 0 || !is_tcp()) return *this;
    _keepidle = sec;
    setsockopt(_fd, IPPROTO_TCP, TCP_KEEPIDLE, &sec, sizeof(sec));
    return *this;
}

SkllProtocol& SkllProtocol::set_keepintvl(int sec) {
    if (_fd < 0 || !is_tcp()) return *this;
    _keepintvl = sec;
    setsockopt(_fd, IPPROTO_TCP, TCP_KEEPINTVL, &sec, sizeof(sec));
    return *this;
}

SkllProtocol& SkllProtocol::set_keepcnt(int count) {
    if (_fd < 0 || !is_tcp()) return *this;
    setsockopt(_fd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
    return *this;
}

SkllProtocol& SkllProtocol::set_maxseg(int size) {
    if (_fd < 0 || !is_tcp()) return *this;
    setsockopt(_fd, IPPROTO_TCP, TCP_MAXSEG, &size, sizeof(size));
    return *this;
}

SkllProtocol& SkllProtocol::set_defer_accept(int sec) {
    if (_fd < 0 || !is_tcp()) return *this;
    setsockopt(_fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &sec, sizeof(sec));
    return *this;
}

SkllProtocol& SkllProtocol::set_fastopen(int qlen) {
    if (_fd < 0 || !is_tcp()) return *this;
    setsockopt(_fd, IPPROTO_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));
    return *this;
}

SkllProtocol& SkllProtocol::set_congestion(const std::string& algo) {
    if (_fd < 0 || !is_tcp()) return *this;
    setsockopt(_fd, IPPROTO_TCP, TCP_CONGESTION, algo.c_str(), algo.size());
    return *this;
}

SkllProtocol& SkllProtocol::set_user_timeout(int ms) {
    if (_fd < 0 || !is_tcp()) return *this;
    setsockopt(_fd, IPPROTO_TCP, TCP_USER_TIMEOUT, &ms, sizeof(ms));
    return *this;
}

SkllProtocol& SkllProtocol::set_tos(int tos) {
    if (_fd < 0) return *this;
    setsockopt(_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    return *this;
}

SkllProtocol& SkllProtocol::set_ttl(int ttl) {
    if (_fd < 0) return *this;
    setsockopt(_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    return *this;
}

SkllProtocol& SkllProtocol::set_v6only(bool enable) {
    if (_fd < 0 || !is_ipv6()) return *this;
    int opt = enable ? 1 : 0;
    setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
    return *this;
}

SkllProtocol& SkllProtocol::set_buffer_size(int size) {
    _buffer_size = size;
    return *this;
}

SkllProtocol& SkllProtocol::set_chunk_size(int size) {
    _chunk_size = size;
    return *this;
}

SkllProtocol& SkllProtocol::set_crlf(const std::string& crlf) {
    _crlf = crlf;
    return *this;
}

SkllProtocol& SkllProtocol::set_timeout(int sec, int keepidle, int keepintvl) {
    _timeout_sec = sec;
    if (keepidle > 0) set_keepidle(keepidle);
    if (keepintvl > 0) set_keepintvl(keepintvl);
    return *this;
}

// ═══════════════════════════════════════════════════
// GETTERS
// ═══════════════════════════════════════════════════
int SkllProtocol::get_fd() const { return _fd; }
std::string SkllProtocol::get_name() const { return _name; }
std::string SkllProtocol::get_address() const { return _address; }
int SkllProtocol::get_port() const { return _port; }
int SkllProtocol::get_options() const { return _options; }
bool SkllProtocol::get_bound() const { return _bound; }
int SkllProtocol::get_buffer_size() const { return _buffer_size; }
int SkllProtocol::get_chunk_size() const { return _chunk_size; }
std::string SkllProtocol::get_crlf() const { return _crlf; }
int SkllProtocol::get_timeout() const { return _timeout_sec; }

bool SkllProtocol::is_tcp() const { return (_options & SKLL_TCP) != 0; }
bool SkllProtocol::is_udp() const { return (_options & SKLL_UDP) != 0; }
bool SkllProtocol::is_ipv4() const { return (_options & SKLL_IPV4) != 0; }
bool SkllProtocol::is_ipv6() const { return (_options & SKLL_IPV6) != 0; }

// ═══════════════════════════════════════════════════
// MESSAGES D'ERREUR
// ═══════════════════════════════════════════════════
std::string SkllProtocol::_get_socket_error() {
    std::ostringstream oss;
    oss << "socket() failed: ";
    switch (errno) {
        case EACCES:       oss << "Permission denied"; break;
        case EAFNOSUPPORT: oss << "Address family not supported"; break;
        case EMFILE:       oss << "Process FD limit reached"; break;
        case ENFILE:       oss << "System FD limit reached"; break;
        case ENOMEM:       oss << "Insufficient memory"; break;
        default: oss << std::strerror(errno);
    }
    return oss.str();
}

std::string SkllProtocol::_get_bind_error() {
    std::ostringstream oss;
    oss << "bind(" << _address << ":" << _port << ") failed: ";
    switch (errno) {
        case EADDRINUSE:    oss << "Address already in use"; break;
        case EACCES:        oss << "Permission denied (port < 1024?)"; break;
        case EADDRNOTAVAIL: oss << "Address not available"; break;
        default: oss << std::strerror(errno);
    }
    return oss.str();
}

std::string SkllProtocol::_get_listen_error() {
    std::ostringstream oss;
    oss << "listen() failed: ";
    switch (errno) {
        case EADDRINUSE: oss << "Address in use"; break;
        default: oss << std::strerror(errno);
    }
    return oss.str();
}