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

SkllHook::SkllHook() : _lib_data(NULL) {}
SkllHook::~SkllHook() {}

SkllHook& SkllHook::on(SkllEvent event, SkllCallback callback, void* user_data) {
    _callbacks[event] = callback;
    if (user_data) _user_data[event] = user_data;
    return *this;
}

void SkllHook::trigger(SkllEvent event, void* lib_data) {
    std::map<SkllEvent, SkllCallback>::iterator it = _callbacks.find(event);
    if (it == _callbacks.end() || !it->second) return;
    
    SkllHookData data;
    data.lib_data = lib_data ? lib_data : _lib_data;
    
    std::map<SkllEvent, void*>::iterator ud = _user_data.find(event);
    if (ud != _user_data.end()) data.user_data = ud->second;
    
    it->second(&data);
}

void SkllHook::set_lib_data(void* data) {
    _lib_data = data;
}