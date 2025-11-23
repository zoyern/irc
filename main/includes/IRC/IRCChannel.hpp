/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannel.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:14:18 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:14:18 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllChannel.hpp>
#include <Sockell/SkllHook.hpp>
#include <string>
#include <set>

class IRCChannel {
public:
    SkllChannel *skll_channel;
    SkllHook hook;
    
    std::string topic;
    std::string password;
    int user_limit;
    
    bool invite_only;
    bool topic_protected;
    bool read_only;
    
    std::set<int> operators;
    std::set<int> invited;
    
    IRCChannel();
    IRCChannel(const std::string &name);
    ~IRCChannel();
    
    IRCChannel &on(int event, SkllHook::Callback cb, void *user_data = NULL);
    
    void broadcast(const std::string &msg, int exclude_fd = -1);
    void send_topic(int fd);
    void send_names(int fd);
    
    bool is_operator(int fd) const;
    void set_operator(int fd, bool op);
    
    bool is_invited(int fd) const;
    void invite(int fd);
    
    bool has_password() const;
    bool check_password(const std::string &pass) const;
    
    bool is_full() const;
    bool can_join(int fd, const std::string &key) const;
    bool can_send(int fd) const;
    
    const std::string &get_name() const;
    size_t get_client_count() const;
    
    static void on_client_join(void *event, void *user_data);
    static void on_client_part(void *event, void *user_data);
    static void on_client_message(void *event, void *user_data);
};