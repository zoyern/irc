/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllChannels.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 00:55:11 by marvin            #+#    #+#             */
/*   Updated: 2025/11/29 00:55:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllChannels.hpp>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLCHANNELS - Collection with REVERSE INDEX for O(k) operations          */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllChannels::SkllChannels() {}

SkllChannels::SkllChannels(const SkllChannels &other)
	: _channels(other._channels), _fd_channels(other._fd_channels) {}

SkllChannels &SkllChannels::operator=(const SkllChannels &other) {
	if (this != &other) {
		_channels = other._channels;
		_fd_channels = other._fd_channels;
	}
	return *this;
}

SkllChannels::~SkllChannels() {}

/* O(n) where n = name length */
std::string SkllChannels::_normalize(const std::string &s) const {
	std::string lower;
	lower.reserve(s.size());
	for (size_t i = 0; i < s.size(); ++i)
		lower += static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
	return lower;
}

/* O(log n) - map insert */
void SkllChannels::_index_add(fd_t fd, const std::string &chan) {
	_fd_channels[fd].insert(chan);
}

/* O(log n) - map/set erase */
void SkllChannels::_index_remove(fd_t fd, const std::string &chan) {
	std::map<fd_t, std::set<std::string> >::iterator it = _fd_channels.find(fd);
	if (it != _fd_channels.end()) {
		it->second.erase(chan);
		if (it->second.empty()) _fd_channels.erase(it);
	}
}

/* O(log n) - map find/insert */
SkllChannel *SkllChannels::create(const std::string &name) {
	std::string key = _normalize(name);
	std::map<std::string, SkllChannel>::iterator it = _channels.find(key);
	if (it != _channels.end()) return &it->second;
	_channels.insert(std::make_pair(key, SkllChannel(name)));
	return &_channels.find(key)->second;
}

/* O(log n) - map find */
SkllChannel *SkllChannels::get(const std::string &name) {
	std::map<std::string, SkllChannel>::iterator it = _channels.find(_normalize(name));
	return (it != _channels.end()) ? &it->second : 0;
}

const SkllChannel *SkllChannels::get(const std::string &name) const {
	std::map<std::string, SkllChannel>::const_iterator it = _channels.find(_normalize(name));
	return (it != _channels.end()) ? &it->second : 0;
}

bool SkllChannels::exists(const std::string &name) const {
	return _channels.find(_normalize(name)) != _channels.end();
}

void SkllChannels::remove(const std::string &name) {
	std::string key = _normalize(name);
	std::map<std::string, SkllChannel>::iterator it = _channels.find(key);
	if (it != _channels.end()) {
		/* Remove from reverse index for all members */
		const std::set<fd_t> &members = it->second.members();
		for (std::set<fd_t>::const_iterator m = members.begin(); m != members.end(); ++m)
			_index_remove(*m, key);
		_channels.erase(it);
	}
}

size_t SkllChannels::count() const { return _channels.size(); }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   OPTIMIZED JOIN/PART with reverse index                                     */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* O(log n) - uses reverse index */
void SkllChannels::join(fd_t fd, const std::string &chan_name, size_t perms) {
	std::string key = _normalize(chan_name);
	SkllChannel *ch = get(chan_name);
	if (!ch) ch = create(chan_name);
	if (ch->add(fd, perms)) {
		_index_add(fd, key);
	}
}

/* O(log n) - uses reverse index */
void SkllChannels::part(fd_t fd, const std::string &chan_name) {
	std::string key = _normalize(chan_name);
	SkllChannel *ch = get(chan_name);
	if (ch) {
		ch->remove(fd);
		_index_remove(fd, key);
	}
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   O(k) OPERATIONS - where k = number of channels user is in                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* O(k *log n) with index, O(n *log m) fallback for direct ch->add() usage */
void SkllChannels::remove_all(fd_t fd) {
	std::map<fd_t, std::set<std::string> >::iterator idx = _fd_channels.find(fd);
	if (idx != _fd_channels.end()) {
		/* Fast path: use reverse index */
		std::set<std::string> chans = idx->second;
		for (std::set<std::string>::iterator it = chans.begin(); it != chans.end(); ++it) {
			std::map<std::string, SkllChannel>::iterator ch = _channels.find(*it);
			if (ch != _channels.end()) ch->second.remove(fd);
		}
		_fd_channels.erase(fd);
	} else {
		/* Fallback: scan all channels (for direct ch->add() usage) */
		for (std::map<std::string, SkllChannel>::iterator it = _channels.begin(); 
		     it != _channels.end(); ++it) {
			it->second.remove(fd);
		}
	}
}

/* O(1) with index, O(n *log m) fallback for direct ch->add() usage */
void SkllChannels::of(fd_t fd, std::set<std::string> &result) const {
	result.clear();
	std::map<fd_t, std::set<std::string> >::const_iterator idx = _fd_channels.find(fd);
	if (idx != _fd_channels.end()) {
		/* Fast path: use reverse index */
		for (std::set<std::string>::const_iterator n = idx->second.begin(); n != idx->second.end(); ++n) {
			std::map<std::string, SkllChannel>::const_iterator ch = _channels.find(*n);
			if (ch != _channels.end()) result.insert(ch->second.name());
		}
	} else {
		/* Fallback: scan all channels (for direct ch->add() usage) */
		for (std::map<std::string, SkllChannel>::const_iterator it = _channels.begin();
		     it != _channels.end(); ++it) {
			if (it->second.has(fd)) result.insert(it->second.name());
		}
	}
}

std::map<std::string, SkllChannel>& SkllChannels::all() { return (_channels); }
const std::map<std::string, SkllChannel>& SkllChannels::all() const { return (_channels); }

/* O(1) - direct access to reverse index */
const std::set<std::string> *SkllChannels::channels_of(fd_t fd) const {
	std::map<fd_t, std::set<std::string> >::const_iterator it = _fd_channels.find(fd);
	return (it != _fd_channels.end()) ? &it->second : 0;
}

SkllChannels::iterator SkllChannels::begin() { return _channels.begin(); }
SkllChannels::iterator SkllChannels::end() { return _channels.end(); }
SkllChannels::const_iterator SkllChannels::begin() const { return _channels.begin(); }
SkllChannels::const_iterator SkllChannels::end() const { return _channels.end(); }

void SkllChannels::clear() {
	_channels.clear();
	_fd_channels.clear();
}
