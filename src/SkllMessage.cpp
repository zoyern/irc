/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 22:04:25 by marvin            #+#    #+#             */
/*   Updated: 2025/11/16 22:04:25 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllMessage.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>

SkllMessage::SkllMessage()
	: _protocol(NULL), _client_fd(-1), _read_pos(0)
	, _tcp_nodelay(true), _tcp_cork(false), _tcp_quickack(true) {}

SkllMessage::SkllMessage(SkllProtocol *proto, int fd)
	: _protocol(proto), _client_fd(fd), _read_pos(0)
	, _tcp_nodelay(true), _tcp_cork(false), _tcp_quickack(true) {}

SkllMessage::SkllMessage(const uint8_t *data, size_t len, SkllProtocol *proto, int fd)
    : _data(data, data + len), _protocol(proto), _client_fd(fd), _read_pos(0)
    , _tcp_nodelay(false), _tcp_cork(false), _tcp_quickack(false) {}

SkllMessage::~SkllMessage() {}

SkllMessage SkllMessage::create_fast(SkllProtocol *proto, int fd) {
	SkllMessage msg(proto, fd);
	msg.set_nodelay(true);
	msg.set_quickack(true);
	msg.set_cork(false);
	return (msg);
}

SkllMessage &SkllMessage::operator<<(const std::string &str) {
    const uint8_t *bytes = (const uint8_t*)str.c_str();
    _data.insert(_data.end(), bytes, bytes + str.size());
    return *this;
}

SkllMessage &SkllMessage::operator<<(const char *str) {
    size_t len = std::strlen(str);
    const uint8_t *bytes = (const uint8_t*)str;
    _data.insert(_data.end(), bytes, bytes + len);
    return *this;
}

SkllMessage &SkllMessage::operator<<(char c) {
    _data.push_back(static_cast<uint8_t>(c));
    return *this;
}

SkllMessage &SkllMessage::operator<<(int value) {
    std::ostringstream oss;
    oss << value;
    return *this << oss.str();
}

SkllMessage &SkllMessage::operator<<(unsigned int value) {
    std::ostringstream oss;
    oss << value;
    return *this << oss.str();
}

SkllMessage &SkllMessage::operator<<(long value) {
    std::ostringstream oss;
    oss << value;
    return *this << oss.str();
}

SkllMessage &SkllMessage::operator<<(bool value) {
    return *this << (value ? "true" : "false");
}

SkllMessage &SkllMessage::operator<<(double value) {
    std::ostringstream oss;
    oss << value;
    return *this << oss.str();
}

std::string SkllMessage::to() {
    skip_spaces();
    return std::string((const char*)&_data[_read_pos], _data.size() - _read_pos);
}

void SkllMessage::skip(size_t n) {
    _read_pos = (_read_pos + n > _data.size()) ? _data.size() : _read_pos + n;
}

void SkllMessage::skip_spaces() {
    while (_read_pos < _data.size() && _data[_read_pos] == ' ') {
        _read_pos++;
    }
}

void SkllMessage::rewind() {
    _read_pos = 0;
}

bool SkllMessage::has_more() const {
    return _read_pos < _data.size();
}

char SkllMessage::peek() const {
    if (_read_pos < _data.size()) {
        return (char)_data[_read_pos];
    }
    return '\0';
}

bool SkllMessage::starts_with(const char *pattern) const {
    size_t plen = std::strlen(pattern);
    if (_read_pos + plen > _data.size()) return false;
    return std::memcmp(&_data[_read_pos], pattern, plen) == 0;
}

void SkllMessage::send() {
    if (_client_fd >= 0) {
        send_to(_client_fd);
    } else if (_protocol) {
        _protocol->broadcast(*this);
    }
}

void SkllMessage::send_to(int fd) {
    if (fd < 0 || _data.empty()) return;
    
    apply_tcp_options();
    ::send(fd, &_data[0], _data.size(), 0);
}

SkllMessage &SkllMessage::set_nodelay(bool enable) {
    _tcp_nodelay = enable;
    return *this;
}

SkllMessage &SkllMessage::set_cork(bool enable) {
    _tcp_cork = enable;
    return *this;
}

SkllMessage &SkllMessage::set_quickack(bool enable) {
    _tcp_quickack = enable;
    return *this;
}

void SkllMessage::apply_tcp_options() {
    if (_client_fd < 0) return;
    
    int val = _tcp_nodelay ? 1 : 0;
    setsockopt(_client_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}

const uint8_t *SkllMessage::data() const {
    return _data.empty() ? NULL : &_data[0];
}

size_t SkllMessage::size() const {
    return _data.size();
}

void SkllMessage::clear() {
    _data.clear();
    _read_pos = 0;
}

size_t SkllMessage::read_pos() const {
    return _read_pos;
}

void SkllMessage::set_protocol(SkllProtocol *proto) {
    _protocol = proto;
}

void SkllMessage::set_fd(int fd) {
    _client_fd = fd;
}

SkllProtocol *SkllMessage::get_protocol() const {
    return _protocol;
}

int SkllMessage::get_fd() const {
    return _client_fd;
}