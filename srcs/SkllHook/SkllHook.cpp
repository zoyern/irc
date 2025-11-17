/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hook.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 00:50:10 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 00:50:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllHook.hpp>

SkllHook::SkllHook() {}

SkllHook::SkllHook(const SkllHook& other)
    : _callbacks(other._callbacks), _userdata(other._userdata) {}

SkllHook& SkllHook::operator=(const SkllHook& other) {
    if (this != &other) {
        _callbacks = other._callbacks;
        _userdata = other._userdata;
    }
    return *this;
}

SkllHook::~SkllHook() { clear(); }

SkllHook& SkllHook::on(int event, Callback callback, void* user_data) {
    _callbacks[event] = callback;
    if (user_data) _userdata[event] = user_data;
    return *this;
}

void SkllHook::off(int event) {
    _callbacks.erase(event);
    _userdata.erase(event);
}

void SkllHook::trigger(int event, void* lib_data) {
    std::map<int, Callback>::iterator it = _callbacks.find(event);
    if (it == _callbacks.end() || !it->second) return;
    
    void* user = NULL;
    std::map<int, void*>::iterator ud = _userdata.find(event);
    if (ud != _userdata.end()) user = ud->second;
    
    it->second(lib_data, user);
}

bool SkllHook::has(int event) const {
    return _callbacks.find(event) != _callbacks.end();
}

void SkllHook::clear() {
    _callbacks.clear();
    _userdata.clear();
}

size_t SkllHook::count() const {
    return _callbacks.size();
}