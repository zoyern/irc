/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lobby.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:37:00 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:37:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllHook.hpp>
#include <string>
#include <cstddef>

class SkllChannel {
public:
    SkllHook    hook;
    std::string name;
    std::string topic;
    std::string password;
    
    SkllChannel();
    SkllChannel(const std::string& channel_name);
    ~SkllChannel();
    
    // Copie (public pour std::map)
    SkllChannel(const SkllChannel& other);
    SkllChannel& operator=(const SkllChannel& other);
    
    SkllChannel& set_topic(const std::string& t);
    SkllChannel& set_password(const std::string& p);
    SkllChannel& op(int flags);
    SkllChannel& size(int max_users);
    SkllChannel& invite_only(bool enable);
    SkllChannel& restricted_topic(bool enable);
};