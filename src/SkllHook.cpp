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

SkllHook::Entry::Entry() : mask(0), callback(NULL), user_data(NULL) {}

SkllHook::Entry::Entry(int m, Callback cb, void *data)
    : mask(m), callback(cb), user_data(data) {}

SkllHook::SkllHook() {}

SkllHook::~SkllHook() {}

void SkllHook::on(int mask, Callback callback, void *user_data) {
    _entries.push_back(Entry(mask, callback, user_data));
}

void SkllHook::trigger(int event_type, void *event) {
    for (size_t i = 0; i < _entries.size(); ++i) {
        if (_entries[i].mask & event_type) {
            if (_entries[i].callback) {
                _entries[i].callback(event, _entries[i].user_data);
            }
        }
    }
}

void SkllHook::clear() {
    _entries.clear();
}