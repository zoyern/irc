/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllProtocol.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 21:40:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/08 21:40:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllMessage.hpp>
#include <Sockell/SkllRouter.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>

class SkllNetwork;

struct SkllProtocolOptions {
    bool reuse_addr;
    bool reuse_port;
    int send_buf_size;
    int recv_buf_size;
    int send_timeout;
    int recv_timeout;
    int buffer_size;
    int chunk_size;
    
    SkllProtocolOptions();
};

struct SkllProtocolTCPOptions {
    bool nodelay;
    bool cork;
    bool quickack;
    bool keepalive;
    int keepalive_time;
    int keepalive_intvl;
    int keepalive_probes;
    
    SkllProtocolTCPOptions();
};

class SkllProtocol {
private:
    int _fd;
    std::string _name;
    std::string _addr;
    int _port;
    int _opts;
    
    bool _started;
    bool _running;
    bool _stopped;
    
    SkllNetwork *_network;
    SkllProtocolOptions _options;
    SkllProtocolTCPOptions _tcp_options;
    
    std::map<int, SkllClient*> _clients;
    SkllRouter _router;
    
public:
	SkllHook _hook;
    SkllProtocol();
    SkllProtocol(const std::string &name, const std::string &addr, int port, int opts);
    ~SkllProtocol();
    
    int start();
    int run();
    int stop();
    int shutdown();
    
    SkllProtocol &on(int event, SkllHook::Callback callback, void *user_data = NULL);
    
    SkllRouter &router();
    
    void add_client(int fd, SkllClient *client);
    void remove_client(int fd);
    SkllClient *get_client(int fd);
    const std::map<int, SkllClient*> &get_clients() const;
    
    void send(int fd, SkllMessage &msg);
    void send(int fd, const std::string &data);
    void broadcast(SkllMessage &msg);
    void broadcast(const std::string &data);
    void on_recv_data(int fd, const uint8_t *data, size_t len);
    
    SkllProtocol &set_reuseaddr(bool enable = true);
    SkllProtocol &set_nodelay(bool enable = true);
    SkllProtocol &set_keepalive(bool enable = true);
    SkllProtocol &set_buffer_sizes(int send_size, int recv_size);
    SkllProtocol &set_chunk_size(int size);
    
    void set_network(SkllNetwork *network);
    SkllNetwork *get_network() const;
    
    int get_fd() const;
    const std::string &get_name() const;
    const std::string &get_address() const;
    int get_port() const;
    bool is_tcp() const;
    bool is_udp() const;
    bool is_started() const;
    bool is_running() const;
    bool is_stopped() const;
    
private:
    void apply_socket_options();
    void bind_and_listen();
    void trigger_event(int type, SkllClient *client = NULL,
                      SkllMessage *msg = NULL, int error = 0);
    
    SkllProtocol(const SkllProtocol&);
    SkllProtocol &operator=(const SkllProtocol&);
};