/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllProtocol.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 15:27:42 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 15:27:42 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllException.hpp>
#include <Sockell/SkllLog.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

SkllProtocol::SkllProtocol()
	: _flags(SKLL_TCP | SKLL_IPV4), _port(0), _host("0.0.0.0")
	, _reuseaddr(true), _nodelay(false), _keepalive(false)
	, _backlog(128), _fd(-1) {}

SkllProtocol::SkllProtocol(int flags, int port)
	: _flags(flags), _port(port), _host("0.0.0.0")
	, _reuseaddr(true), _nodelay(false), _keepalive(false)
	, _backlog(128), _fd(-1) {}

SkllProtocol::~SkllProtocol() { close(); }

SkllProtocol &SkllProtocol::set_flags(int f) { _flags = f; return *this; }
SkllProtocol &SkllProtocol::set_port(int p) { _port = p; return *this; }
SkllProtocol &SkllProtocol::set_host(const std::string &h) { _host = h; return *this; }
SkllProtocol &SkllProtocol::set_reuseaddr(bool v) { _reuseaddr = v; return *this; }
SkllProtocol &SkllProtocol::set_nodelay(bool v) { _nodelay = v; return *this; }
SkllProtocol &SkllProtocol::set_keepalive(bool v) { _keepalive = v; return *this; }
SkllProtocol &SkllProtocol::set_backlog(int b) { _backlog = b; return *this; }

int SkllProtocol::flags() const { return _flags; }
int SkllProtocol::port() const { return _port; }
const std::string &SkllProtocol::host() const { return _host; }
bool SkllProtocol::reuseaddr() const { return _reuseaddr; }
bool SkllProtocol::nodelay() const { return _nodelay; }
bool SkllProtocol::keepalive() const { return _keepalive; }
int SkllProtocol::backlog() const { return _backlog; }
fd_t SkllProtocol::fd() const { return _fd; }

bool SkllProtocol::is_tcp() const { return (_flags &SKLL_TCP) != 0; }
bool SkllProtocol::is_udp() const { return (_flags &SKLL_UDP) != 0; }
bool SkllProtocol::is_ipv4() const { return (_flags &SKLL_IPV4) != 0; }
bool SkllProtocol::is_ipv6() const { return (_flags &SKLL_IPV6) != 0; }
bool SkllProtocol::bound() const { return _fd >= 0; }

bool SkllProtocol::bind() {
	/* Validate port */
	if (_port < 1 || _port > 65535)
		throw SkllPortException();
	
	int domain = is_ipv6() ? AF_INET6 : AF_INET;
	int type = is_udp() ? SOCK_DGRAM : SOCK_STREAM;
	
	_fd = socket(domain, type, 0);
	if (_fd < 0)
		throw SkllSocketException();
	
	if (_reuseaddr) {
		int opt = 1;
		setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	}
	
	/* For IPv6, allow both IPv4 and IPv6 connections if dual-stack requested */
	if (is_ipv6() && is_ipv4()) {
		int opt = 0; /* IPV6_V6ONLY = 0 allows dual-stack */
		setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
	}
	
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	
	if (is_ipv6()) {
		/* IPv6 bind */
		struct sockaddr_in6 addr6;
		std::memset(&addr6, 0, sizeof(addr6));
		addr6.sin6_family = AF_INET6;
		addr6.sin6_port = htons(_port);
		if (_host == "0.0.0.0" || _host == "::") {
			addr6.sin6_addr = in6addr_any;
		} else {
			inet_pton(AF_INET6, _host.c_str(), &addr6.sin6_addr);
		}
		
		if (::bind(_fd, reinterpret_cast<struct sockaddr *>(&addr6), sizeof(addr6)) < 0) {
			::close(_fd);
			_fd = -1;
			throw SkllBindException();
		}
	} else {
		/* IPv4 bind */
		struct sockaddr_in addr4;
		std::memset(&addr4, 0, sizeof(addr4));
		addr4.sin_family = AF_INET;
		addr4.sin_port = htons(_port);
		addr4.sin_addr.s_addr = inet_addr(_host.c_str());
		
		if (::bind(_fd, reinterpret_cast<struct sockaddr *>(&addr4), sizeof(addr4)) < 0) {
			::close(_fd);
			_fd = -1;
			throw SkllBindException();
		}
	}
	
	if (is_tcp() && ::listen(_fd, _backlog) < 0) {
		::close(_fd);
		_fd = -1;
		throw SkllListenException();
	}
	
	std::ostringstream oss;
	oss << "Listening on " << (is_ipv6() ? "[" + _host + "]" : _host) << ":" << _port;
	if (is_ipv6() && is_ipv4()) oss << " (dual-stack)";
	SkllLog::success(oss.str());
	return true;
}

void SkllProtocol::close() {
	if (_fd >= 0) {
		::close(_fd);
		_fd = -1;
	}
}
