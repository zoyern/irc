/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllSender.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:00:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 20:00:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllSender.hpp>
#include <sys/socket.h>

/* SkllSender - Auto-flush on destruction */

SkllSender::SkllSender(fd_t fd, const std::string &delim)
	: _fd(fd), _client(0), _delim(delim), _flushed(false) {}

SkllSender::SkllSender(SkllClient *client, const std::string &delim)
	: _fd(client ? client->fd() : -1), _client(client), _delim(delim), _flushed(false) {}

/* Copy constructor - transfers ownership (source becomes flushed to prevent double-flush) */
SkllSender::SkllSender(const SkllSender &other)
	: _fd(other._fd), _client(other._client), _delim(other._delim), _buf(other._buf), _flushed(other._flushed) {
	/* Mark source as flushed to prevent double-flush on destruction */
	const_cast<SkllSender &>(other)._flushed = true;
	const_cast<SkllSender &>(other)._buf.clear();
}

SkllSender::~SkllSender() {
	if (!_flushed && !_buf.empty()) flush();
}

SkllSender &SkllSender::operator<<(const SkllSpan &span) {
	_buf += span.str();
	return *this;
}

SkllSender &SkllSender::operator<<(const char *str) {
	if (str) _buf += str;
	return *this;
}

void SkllSender::flush() {
	if (_buf.empty() || _fd < 0) return;
	std::string data = _buf + _delim;
	
	if (_client && _client->is_udp()) {
		/* UDP - use sendto with stored client address */
		sendto(_fd, data.c_str(), data.size(), MSG_NOSIGNAL,
		       _client->udp_addr(), _client->udp_addr_len());
	} else {
		/* TCP - use send */
		::send(_fd, data.c_str(), data.size(), MSG_NOSIGNAL);
	}
	
	_buf.clear();
	_flushed = true;
}

void SkllSender::clear() {
	_buf.clear();
}

void SkllSender::set_delim(const std::string &d) { _delim = d; }

/* SkllBroadcast */

SkllBroadcast::SkllBroadcast(const std::string &delim)
	: _except(-1), _delim(delim) {}

SkllBroadcast::~SkllBroadcast() {}

SkllBroadcast &SkllBroadcast::to(fd_t fd) {
	_targets.insert(fd);
	return *this;
}

SkllBroadcast &SkllBroadcast::to(const std::set<fd_t> &fds) {
	_targets.insert(fds.begin(), fds.end());
	return *this;
}

SkllBroadcast &SkllBroadcast::except(fd_t fd) {
	_except = fd;
	return *this;
}

SkllBroadcast &SkllBroadcast::operator<<(const SkllSpan &span) {
	_buf << span.str();
	return *this;
}

SkllBroadcast &SkllBroadcast::operator<<(const char *str) {
	if (str) _buf << str;
	return *this;
}

void SkllBroadcast::flush() {
	std::string data = _buf.str();
	if (data.empty()) return;
	data += _delim;
	for (std::set<fd_t>::iterator it = _targets.begin(); it != _targets.end(); ++it)
		if (*it != _except)
			::send(*it, data.c_str(), data.size(), MSG_NOSIGNAL);
	_buf.str("");
	_buf.clear();
}

void SkllBroadcast::clear() {
	_targets.clear();
	_except = -1;
	_buf.str("");
	_buf.clear();
}
