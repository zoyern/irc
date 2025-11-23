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
#include <vector>
#include <cstddef>

class SkllHook {
public:
    typedef void (*Callback)(void *event, void *user_data);
    
    struct Entry {
        int mask;
        Callback callback;
        void *user_data;
        
        Entry();
        Entry(int m, Callback cb, void *data);
    };
    
private:
    std::vector<Entry> _entries;
    
public:
    SkllHook();
    ~SkllHook();
    
    void on(int mask, Callback callback, void *user_data = NULL);
    void trigger(int event_type, void *event);
    void clear();
};