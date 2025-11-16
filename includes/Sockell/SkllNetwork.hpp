/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllNetwork.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 16:27:41 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 16:27:41 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllServer.hpp>
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <iostream>

class SkllServer;

class SkllNetwork {
private:
    std::string _name;
    int  _epfd;
    int  _timeout;
    int  _queue;
    uint32_t _epoll_events;
    
    SkllServer* _server;
    
    std::vector<epoll_event> _events;
    std::map<std::string, SkllProtocol>  _protocols_owned;
    std::map<int, SkllProtocol*>         _fd_to_protocol;

public:
    SkllHook hook;
    
    SkllNetwork();
    SkllNetwork(const std::string& name, int timeout, int queue);
    ~SkllNetwork();
    
    // ═══════════════════════════════════════════════════
    // COPIE (PUBLIC pour std::map)
    // ═══════════════════════════════════════════════════
    SkllNetwork(const SkllNetwork& other);
    SkllNetwork& operator=(const SkllNetwork& other);
    
    void run();
    void update();
    void set_server(SkllServer* srv);
    
    SkllProtocol& listen(const std::string& name, const std::string& addr, int port, int opts);
    SkllProtocol& listen(SkllProtocol* proto);
    
    SkllNetwork& on(SkllEvent event, SkllCallback callback, void* data = NULL);
    
    // Setters
    SkllNetwork& set_timeout(int milliseconds);
    SkllNetwork& set_queue(int size);
    SkllNetwork& set_events(uint32_t events);
    SkllNetwork& set_edge_triggered(bool enable = true);
    SkllNetwork& set_oneshot(bool enable = true);
    SkllNetwork& set_exclusive(bool enable = true);
    
    // Getters
    std::string get_name() const;
    int         get_epfd() const;
    int         get_timeout() const;
    int         get_queue() const;
    uint32_t    get_events() const;
    int         count_protocols() const;
    
    void add_event(int fd);
    void destroy_event(int fd);

private:
    void _handle_listening(int fd, uint32_t events);
    void _handle_client(int fd, uint32_t events);
    void _accept_tcp(int listen_fd);
    void _recv_tcp(int fd);
    
    std::string _get_epoll_error();
};