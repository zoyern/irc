/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 22:03:59 by marvin            #+#    #+#             */
/*   Updated: 2025/11/16 22:03:59 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <cstddef>

class SkllProtocol;

class SkllMessage {
private:
	std::vector<uint8_t> _data;
	SkllProtocol *_protocol;
	int _client_fd;
	mutable size_t _read_pos;
	
	bool _tcp_nodelay;
	bool _tcp_cork;
	bool _tcp_quickack;
	
public:
	SkllMessage();
	SkllMessage(SkllProtocol *proto, int fd = -1);
	SkllMessage(const uint8_t *data, size_t len, SkllProtocol *proto, int fd = -1);
	~SkllMessage();
	
	static SkllMessage create_fast(SkllProtocol *proto, int fd = -1);
	
	SkllMessage &operator<<(const std::string &str);
	SkllMessage &operator<<(const char *str);
	SkllMessage &operator<<(char c);
	SkllMessage &operator<<(int value);
	SkllMessage &operator<<(unsigned int value);
	SkllMessage &operator<<(long value);
	SkllMessage &operator<<(bool value);
	SkllMessage &operator<<(double value);

	std::string to();
	template<typename T> SkllMessage &write(const T &obj);
	template<typename T> T to();
	template<typename T> bool read(T &obj);
	
	void skip(size_t n);
	void skip_spaces();
	void rewind();
	bool has_more() const;
	char peek() const;
	bool starts_with(const char *pattern) const;
	
	void send();
	void send_to(int fd);
	
	SkllMessage &set_nodelay(bool enable = true);
	SkllMessage &set_cork(bool enable = true);
	SkllMessage &set_quickack(bool enable = true);
	
	const uint8_t *data() const;
	size_t size() const;
	void clear();
	size_t read_pos() const;
	
	void set_protocol(SkllProtocol *proto);
	void set_fd(int fd);
	SkllProtocol *get_protocol() const;
	int get_fd() const;
	
private:
	void apply_tcp_options();
};

template<typename T>
T SkllMessage::to() {
	skip_spaces();
	
	size_t start = _read_pos;
	while (_read_pos < _data.size() &&
		   _data[_read_pos] != ' ' &&
		   _data[_read_pos] != '\r' &&
		   _data[_read_pos] != '\n') {
		_read_pos++;
	}
	
	std::string token((const char*)&_data[start], _read_pos - start);
	
	if (sizeof(T) <= sizeof(long)) {
		return static_cast<T>(std::atol(token.c_str()));
	}
	
	std::istringstream iss(token);
	T value;
	iss >> value;
	return value;
}

template<>
inline std::string SkllMessage::to<std::string>() {
	skip_spaces();
	size_t start = _read_pos;
	
	while (_read_pos < _data.size() &&
		   _data[_read_pos] != ' ' &&
		   _data[_read_pos] != '\r' &&
		   _data[_read_pos] != '\n') {
		_read_pos++;
	}
	
	return std::string((const char*)&_data[start], _read_pos - start);
}

template<>
inline bool SkllMessage::to<bool>() {
	std::string str = to<std::string>();
	return str == "true" || str == "1" || str == "yes";
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ✅ Advanced serialization for custom types
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

template<typename T>
SkllMessage &SkllMessage::write(const T &obj) {
	const uint8_t *bytes = reinterpret_cast<const uint8_t*>(&obj);
	_data.insert(_data.end(), bytes, bytes + sizeof(T));
	return *this;
}

template<typename T>
bool SkllMessage::read(T &obj) {
	if (_read_pos + sizeof(T) > _data.size()) {
		return false;
	}
	
	std::memcpy(&obj, &_data[_read_pos], sizeof(T));
	_read_pos += sizeof(T);
	return true;
}