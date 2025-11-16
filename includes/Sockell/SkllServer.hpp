/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:17:56 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:17:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllChannel.hpp>
#include <sys/resource.h>
#include <map>
#include <string>

class SkllNetwork;

class SkllServer {
private:
    int  _clients_max;
    int  _reserved_fd;
    bool _running;
    
    std::map<std::string, SkllNetwork> _networks_owned;
    std::map<std::string, SkllChannel> _channels_owned;

public:
    SkllServer(int max_clients, int reserved_fd);
    ~SkllServer();
    
    int  run();
    void stop();
    
    SkllNetwork& network(const std::string& name, int timeout, int queue);
    SkllChannel& channel(const std::string& name, bool is_default = false);
    
    std::string print_networks() const;
    void update_fd_limits();

private:
    int _count_listening_sockets() const;
    
    SkllServer(const SkllServer&);
    SkllServer& operator=(const SkllServer&);
};