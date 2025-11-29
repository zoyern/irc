/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllBuffer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 22:18:36 by marvin            #+#    #+#             */
/*   Updated: 2025/11/26 22:18:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllBuffer.hpp>

SkllBuffer::SkllBuffer() {}

SkllBuffer::SkllBuffer(const SkllBuffer &other)
	: _line_buf(other._line_buf), _data(other._data) {}

SkllBuffer &SkllBuffer::operator=(const SkllBuffer &other) {
	if (this != &other) {
		_line_buf = other._line_buf;
		_data = other._data;
	}
	return *this;
}

SkllBuffer::~SkllBuffer() {}

bool SkllBuffer::empty() const { return _data.empty(); }
size_t SkllBuffer::size() const { return _data.size(); }

size_t SkllBuffer::write(const byte_t *data, size_t len) {
	for (size_t i = 0; i < len; ++i)
		_data.push_back(data[i]);
	return len;
}

size_t SkllBuffer::write(const char *str) {
	return str ? write(reinterpret_cast<const byte_t *>(str), std::strlen(str)) : 0;
}

size_t SkllBuffer::write(const std::string &str) {
	return write(reinterpret_cast<const byte_t *>(str.c_str()), str.size());
}

size_t SkllBuffer::write(const SkllSpan &span) {
	return span.empty() ? 0 : write(span.data, span.len);
}

size_t SkllBuffer::read(byte_t *dst, size_t len) {
	size_t actual = (len > _data.size()) ? _data.size() : len;
	for (size_t i = 0; i < actual; ++i) {
		dst[i] = _data.front();
		_data.pop_front();
	}
	return actual;
}

size_t SkllBuffer::peek(byte_t *dst, size_t len) const {
	size_t actual = (len > _data.size()) ? _data.size() : len;
	for (size_t i = 0; i < actual; ++i)
		dst[i] = _data[i];
	return actual;
}

void SkllBuffer::skip(size_t len) {
	size_t actual = (len > _data.size()) ? _data.size() : len;
	for (size_t i = 0; i < actual; ++i)
		_data.pop_front();
}

SkllSpan SkllBuffer::line(const byte_t *delim, size_t dlen) {
	ssize_t pos = find(delim, dlen);
	if (pos < 0) return SkllSpan();
	
	_line_buf.clear();
	_line_buf.reserve(pos);
	for (ssize_t i = 0; i < pos; ++i) {
		_line_buf += static_cast<char>(_data.front());
		_data.pop_front();
	}
	skip(dlen);
	return SkllSpan(_line_buf);
}

SkllSpan SkllBuffer::line(const char *delim) {
	return line(reinterpret_cast<const byte_t *>(delim), std::strlen(delim));
}

ssize_t SkllBuffer::find(byte_t c) const {
	for (size_t i = 0; i < _data.size(); ++i)
		if (_data[i] == c) return static_cast<ssize_t>(i);
	return -1;
}

ssize_t SkllBuffer::find(const byte_t *seq, size_t len) const {
	if (len == 0 || len > _data.size()) return -1;
	for (size_t i = 0; i <= _data.size() - len; ++i) {
		bool match = true;
		for (size_t j = 0; j < len && match; ++j)
			if (_data[i + j] != seq[j]) match = false;
		if (match) return static_cast<ssize_t>(i);
	}
	return -1;
}

byte_t SkllBuffer::operator[](size_t i) const {
	return (i < _data.size()) ? _data[i] : 0;
}

void SkllBuffer::reset() { _data.clear(); _line_buf.clear(); }
