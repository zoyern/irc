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

SkllMessage::SkllMessage() : _parsed(false) {}
SkllMessage::~SkllMessage() {}

void SkllMessage::set(const SkllSpan &raw) {
	set(raw.data, raw.len);
}

void SkllMessage::set(const byte_t *data, size_t len) {
	reset();
	_raw.assign(data, data + len);
	_parse();
}

void SkllMessage::_parse() {
	if (_raw.empty()) return;
	_parsed = true;
	
	const byte_t *p = &_raw[0];
	const byte_t *end = p + _raw.size();
	
	/* Skip prefix if present */
	if (*p == ':') {
		++p;
		const byte_t *start = p;
		while (p < end && *p != ' ') ++p;
		_prefix = SkllSpan(start, p - start);
		while (p < end && *p == ' ') ++p;
	}
	
	/* Command */
	const byte_t *cmd_start = p;
	while (p < end && *p != ' ') ++p;
	_cmd = SkllSpan(cmd_start, p - cmd_start);
	
	/* Arguments */
	while (p < end) {
		while (p < end && *p == ' ') ++p;
		if (p >= end) break;
		
		if (*p == ':') {
			++p;
			_trailing = SkllSpan(p, end - p);
			_args.push_back(_trailing);
			break;
		}
		
		const byte_t *arg_start = p;
		while (p < end && *p != ' ') ++p;
		_args.push_back(SkllSpan(arg_start, p - arg_start));
	}
}

SkllSpan SkllMessage::prefix() const { return _prefix; }
SkllSpan SkllMessage::cmd() const { return _cmd; }
size_t SkllMessage::argc() const { return _args.size(); }

SkllSpan SkllMessage::arg(size_t idx) const {
	return (idx < _args.size()) ? _args[idx] : SkllSpan();
}

SkllSpan SkllMessage::trailing() const { return _trailing; }

SkllSpan SkllMessage::raw() const {
	return _raw.empty() ? SkllSpan() : SkllSpan(&_raw[0], _raw.size());
}

void SkllMessage::set_param(const std::string &key, const SkllSpan &val) {
	_params[key] = val;
}

void SkllMessage::set_param(const std::string &key, const std::string &val) {
	_str_params[key] = val;
}

SkllSpan SkllMessage::param(const std::string &key) const {
	std::map<std::string, SkllSpan>::const_iterator it = _params.find(key);
	if (it != _params.end()) return it->second;
	std::map<std::string, std::string>::const_iterator it2 = _str_params.find(key);
	if (it2 != _str_params.end()) return SkllSpan(it2->second);
	return SkllSpan();
}

bool SkllMessage::has_param(const std::string &key) const {
	return _params.find(key) != _params.end() || _str_params.find(key) != _str_params.end();
}

bool SkllMessage::empty() const { return !_parsed || _cmd.empty(); }

void SkllMessage::reset() {
	_raw.clear();
	_prefix = SkllSpan();
	_cmd = SkllSpan();
	_args.clear();
	_trailing = SkllSpan();
	_params.clear();
	_str_params.clear();
	_parsed = false;
}
