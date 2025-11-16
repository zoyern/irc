/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllHook.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:23:17 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:23:17 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <map>
#include <cstddef>

enum SkllEvent {
    ON_START      = 1 << 0,
    ON_UPDATE     = 1 << 1,
    ON_SHUTDOWN   = 1 << 2,
    ON_CONNECT    = 1 << 3,
    ON_DISCONNECT = 1 << 4,
    ON_ERROR      = 1 << 5,
    ON_TIMEOUT    = 1 << 6,
    ON_RECV       = 1 << 7,
    ON_SEND       = 1 << 8
};

struct SkllHookData {
    void* lib_data;
    void* user_data;
    SkllHookData() : lib_data(NULL), user_data(NULL) {}
};

typedef void (*SkllCallback)(SkllHookData* data);

class SkllHook {
private:
    std::map<SkllEvent, SkllCallback> _callbacks;
    std::map<SkllEvent, void*>        _user_data;
    void* _lib_data;

public:
    SkllHook();
    ~SkllHook();
    
    SkllHook& on(SkllEvent event, SkllCallback callback, void* user_data = NULL);
    void trigger(SkllEvent event, void* lib_data = NULL);
    void set_lib_data(void* data);
};