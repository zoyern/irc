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

SkllMessage::SkllMessage(SkllBuffer& b) : _buf(b) {}
SkllMessage::~SkllMessage() {}

bool SkllMessage::has_line(const char* delim, size_t dlen) const {
    if (_buf.size() < dlen) return false;
    
    for (size_t i = 0; i <= _buf.size() - dlen; ++i)
        if (std::memcmp(&_buf[i], delim, dlen) == 0) return true;
    
    if (dlen == 2 && delim[0] == '\r' && delim[1] == '\n')
        for (size_t i = 0; i < _buf.size(); ++i)
            if (_buf[i] == '\n') return true;
    
    return false;
}

size_t SkllMessage::extract_line(SkllBuffer& out, const char* delim, size_t dlen) {
    for (size_t i = 0; i <= _buf.size() - dlen; ++i) {
        if (std::memcmp(&_buf[i], delim, dlen) == 0) {
            out.assign(_buf.begin(), _buf.begin() + i);
            _buf.erase(_buf.begin(), _buf.begin() + i + dlen);
            return out.size();
        }
    }
    
    if (dlen == 2 && delim[0] == '\r' && delim[1] == '\n') {
        for (size_t i = 0; i < _buf.size(); ++i) {
            if (_buf[i] == '\n') {
                out.assign(_buf.begin(), _buf.begin() + i);
                if (!out.empty() && out.back() == '\r') out.pop_back();
                _buf.erase(_buf.begin(), _buf.begin() + i + 1);
                return out.size();
            }
        }
    }
    
    return 0;
}

const unsigned char* SkllMessage::data() const {
    return _buf.empty() ? NULL : &_buf[0];
}

size_t SkllMessage::size() const {
    return _buf.size();
}