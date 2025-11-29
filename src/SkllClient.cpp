/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 03:55:49 by marvin            #+#    #+#             */
/*   Updated: 2025/11/06 03:55:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllClient.hpp>
#include <cstring>

SkllClient::SkllClient() : _fd(-1), _port(0), _data(0), _is_udp(false), _udp_addr_len(0) {
	std::memset(&_udp_addr, 0, sizeof(_udp_addr));
}

SkllClient::SkllClient(const SkllClient &other)
	: _fd(other._fd), _port(other._port),  _data(other._data), _in(), _out(), _ip(other._ip)
	,  _is_udp(other._is_udp), _udp_addr_len(other._udp_addr_len), _udp_addr(other._udp_addr) {}

SkllClient &SkllClient::operator=(const SkllClient &other) {
	if (this != &other) {
		_fd = other._fd;
		_port = other._port;
		_ip = other._ip;
		_data = other._data;
		_udp_addr = other._udp_addr;
		_udp_addr_len = other._udp_addr_len;
		_is_udp = other._is_udp;
	}
	return *this;
}

SkllClient::~SkllClient() {}

void SkllClient::init(fd_t fd) {
	_fd = fd;
	_ip.clear();
	_port = 0;
	_in.reset();
	_out.reset();
	_is_udp = false;
	_udp_addr_len = 0;
	std::memset(&_udp_addr, 0, sizeof(_udp_addr));
}

void SkllClient::reset() {
	_fd = -1;
	_ip.clear();
	_port = 0;
	_in.reset();
	_out.reset();
	_data = 0;
	_is_udp = false;
	_udp_addr_len = 0;
	std::memset(&_udp_addr, 0, sizeof(_udp_addr));
}

bool SkllClient::active() const { return _fd >= 0; }
fd_t SkllClient::fd() const { return _fd; }

void SkllClient::set_addr(const std::string &ip, int port) {
	_ip = ip;
	_port = port;
}

const std::string &SkllClient::ip() const { return _ip; }
int SkllClient::port() const { return _port; }

/* UDP support */
void SkllClient::set_udp_addr(const struct sockaddr_storage &addr, socklen_t len) {
	_udp_addr = addr;
	_udp_addr_len = len;
	_is_udp = true;
}

const struct sockaddr *SkllClient::udp_addr() const {
	return reinterpret_cast<const struct sockaddr *>(&_udp_addr);
}

socklen_t SkllClient::udp_addr_len() const { return _udp_addr_len; }
bool SkllClient::is_udp() const { return _is_udp; }

SkllBuffer &SkllClient::in() { return _in; }
SkllBuffer &SkllClient::out() { return _out; }
const SkllBuffer &SkllClient::in() const { return _in; }
const SkllBuffer &SkllClient::out() const { return _out; }

void *SkllClient::raw_data() { return _data; }
const void *SkllClient::raw_data() const { return _data; }
void SkllClient::set_raw_data(void *ptr) { _data = ptr; }
