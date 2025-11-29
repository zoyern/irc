/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 18:52:14 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 18:52:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllChannel.hpp>
#include <cctype>

std::string SkllChannel::_empty;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLCHANNEL - Single channel                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllChannel::SkllChannel() {}
SkllChannel::SkllChannel(const std::string &name) : _name(name) {}

SkllChannel::SkllChannel(const SkllChannel &other)
	: _name(other._name), _members(other._members), _perms(other._perms), _meta(other._meta) {}

SkllChannel &SkllChannel::operator=(const SkllChannel &other) {
	if (this != &other) {
		_name = other._name;
		_members = other._members;
		_perms = other._perms;
		_meta = other._meta;
	}
	return *this;
}

SkllChannel::~SkllChannel() {}

const std::string &SkllChannel::name() const { return _name; }
void SkllChannel::set_name(const std::string &name) { _name = name; }

/* O(log n) - set insertion */
bool SkllChannel::add(fd_t fd, size_t perms) {
	if (_members.insert(fd).second) {
		_perms[fd] = perms;
		return true;
	}
	return false;
}

/* O(log n) - set/map erase */
void SkllChannel::remove(fd_t fd) {
	_members.erase(fd);
	_perms.erase(fd);
}

/* O(log n) - set find */
bool SkllChannel::has(fd_t fd) const { return _members.find(fd) != _members.end(); }
size_t SkllChannel::count() const { return _members.size(); }
bool SkllChannel::empty() const { return _members.empty(); }
const std::set<fd_t> &SkllChannel::members() const { return _members; }

/* O(log n) - map find */
size_t SkllChannel::perms(fd_t fd) const {
	std::map<fd_t, size_t>::const_iterator it = _perms.find(fd);
	return (it != _perms.end()) ? it->second : 0;
}

void SkllChannel::set_perms(fd_t fd, size_t p) {
	if (has(fd)) _perms[fd] = p;
}

void SkllChannel::add_perms(fd_t fd, size_t p) {
	std::map<fd_t, size_t>::iterator it = _perms.find(fd);
	if (it != _perms.end()) it->second |= p;
}

void SkllChannel::del_perms(fd_t fd, size_t p) {
	std::map<fd_t, size_t>::iterator it = _perms.find(fd);
	if (it != _perms.end()) it->second &= ~p;
}

bool SkllChannel::has_perm(fd_t fd, size_t p) const {
	return (perms(fd) &p) != 0;
}

void SkllChannel::set(const std::string &key, const std::string &val) { _meta[key] = val; }

const std::string &SkllChannel::get(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = _meta.find(key);
	return (it != _meta.end()) ? it->second : _empty;
}

bool SkllChannel::has(const std::string &key) const { return _meta.find(key) != _meta.end(); }
void SkllChannel::del(const std::string &key) { _meta.erase(key); }

void SkllChannel::clear() {
	_members.clear();
	_perms.clear();
	_meta.clear();
}
