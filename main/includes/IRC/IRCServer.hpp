/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:14:52 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:14:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <IRC/IRCClient.hpp>
#include <IRC/IRCChannel.hpp>
#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllHook.hpp>
#include <map>
#include <string>

class IRCServer {
private:
    std::string _password;
    std::string _name;
    
    std::map<int, IRCClient*> _clients;
    std::map<std::string, IRCChannel*> _channels;
    
    SkllHook _hook;
    
public:
    IRCServer(const std::string &password);
    ~IRCServer();

    IRCServer &on(int event, SkllHook::Callback cb, void *user_data = NULL);

    void add_client(int fd, SkllClient *skll_client);
    void remove_client(int fd);
    IRCClient *get_client(int fd);
    IRCClient *get_client_by_nick(const std::string &nickname);

    IRCChannel *get_channel(const std::string &name);
    IRCChannel *create_channel(const std::string &name);
    void remove_channel(const std::string &name);
    void setup_default_channels();

    bool check_password(const std::string &password) const;

    const std::string &get_name() const;

    static void on_server_start(void *event, void *user_data);
    static void on_client_connect(void *event, void *user_data);
    static void on_client_disconnect(void *event, void *user_data);
};