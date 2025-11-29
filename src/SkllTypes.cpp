/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 22:17:14 by marvin            #+#    #+#             */
/*   Updated: 2025/11/26 22:17:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllTypes.hpp>
#include <cstdlib>

SkllSpan::SkllSpan() : data(0), len(0) {}
SkllSpan::SkllSpan(const byte_t *d, size_t l) : data(d), len(l) {}
SkllSpan::SkllSpan(const char *s) : data(reinterpret_cast<const byte_t *>(s)), len(s ? std::strlen(s) : 0) {}
SkllSpan::SkllSpan(const std::string &s) : data(reinterpret_cast<const byte_t *>(s.c_str())), len(s.size()) {}

bool SkllSpan::empty() const { return len == 0 || data == 0; }
size_t SkllSpan::size() const { return len; }

bool SkllSpan::eq(const SkllSpan &other) const {
	if (len != other.len) return false;
	return len == 0 || std::memcmp(data, other.data, len) == 0;
}

bool SkllSpan::eq(const char *s) const { return eq(s, s ? std::strlen(s) : 0); }

bool SkllSpan::eq(const char *s, size_t l) const {
	if (len != l) return false;
	return len == 0 || std::memcmp(data, s, len) == 0;
}

bool SkllSpan::starts(const char *s) const {
	size_t slen = s ? std::strlen(s) : 0;
	return slen <= len && std::memcmp(data, s, slen) == 0;
}

bool SkllSpan::starts(const SkllSpan &other) const {
	return other.len <= len && std::memcmp(data, other.data, other.len) == 0;
}

std::string SkllSpan::str() const {
	return empty() ? std::string() : std::string(reinterpret_cast<const char *>(data), len);
}

int SkllSpan::to_int() const { return static_cast<int>(to_long()); }
long SkllSpan::to_long() const { return empty() ? 0 : std::strtol(str().c_str(), 0, 10); }

SkllSpan SkllSpan::sub(size_t start, size_t length) const {
	if (start >= len) return SkllSpan();
	size_t actual = (start + length > len) ? (len - start) : length;
	return SkllSpan(data + start, actual);
}

SkllSpan SkllSpan::trim() const {
	if (empty()) return SkllSpan();
	size_t s = 0, e = len;
	while (s < e && (data[s] == ' ' || data[s] == '\t' || data[s] == '\r' || data[s] == '\n')) ++s;
	while (e > s && (data[e-1] == ' ' || data[e-1] == '\t' || data[e-1] == '\r' || data[e-1] == '\n')) --e;
	return SkllSpan(data + s, e - s);
}

byte_t SkllSpan::operator[](size_t i) const { return (i < len) ? data[i] : 0; }
const byte_t *SkllSpan::begin() const { return data; }
const byte_t *SkllSpan::end() const { return data + len; }
