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
#include <Sockell/SkllErrors.hpp>
#include <string>
#include <cstddef>

enum SkllProtocolType {
    SKLL_TCP  = 1 << 0,
    SKLL_UDP  = 1 << 1,
    SKLL_IPV4 = 1 << 2,
    SKLL_IPV6 = 1 << 3
};

class SkllProtocol {
private:
    int         _fd;
    std::string _name;
    std::string _address;
    int         _port;
    int         _options;
    bool        _bound;
    
    int         _buffer_size;
    int         _chunk_size;
    std::string _crlf;
    int         _timeout_sec;
    int         _keepidle;
    int         _keepintvl;

public:
    SkllHook hook;
    
    SkllProtocol();
    SkllProtocol(const std::string& name, const std::string& addr, int port, int opts);
    ~SkllProtocol();
    
    // ═══════════════════════════════════════════════════
    // COPIE (PUBLIC pour std::map)
    // ═══════════════════════════════════════════════════
    SkllProtocol(const SkllProtocol& other);
    SkllProtocol& operator=(const SkllProtocol& other);
    
    void create(const std::string& name, const std::string& addr, int port, int opts);
    void run();
    
    // Setters
    SkllProtocol& set_reuseaddr(bool enable = true);
    SkllProtocol& set_reuseport(bool enable = true);
    SkllProtocol& set_keepalive(bool enable = true);
    SkllProtocol& set_nodelay(bool enable = true);
    SkllProtocol& set_quickack(bool enable = true);
    SkllProtocol& set_cork(bool enable = true);
    SkllProtocol& set_linger(bool enable, int seconds);
    SkllProtocol& set_rcvbuf(int size);
    SkllProtocol& set_sndbuf(int size);
    SkllProtocol& set_rcvlowat(int bytes);
    SkllProtocol& set_sndlowat(int bytes);
    SkllProtocol& set_keepidle(int seconds);
    SkllProtocol& set_keepintvl(int seconds);
    SkllProtocol& set_keepcnt(int count);
    SkllProtocol& set_maxseg(int size);
    SkllProtocol& set_defer_accept(int seconds);
    SkllProtocol& set_fastopen(int qlen);
    SkllProtocol& set_congestion(const std::string& algo);
    SkllProtocol& set_user_timeout(int milliseconds);
    SkllProtocol& set_tos(int tos);
    SkllProtocol& set_ttl(int ttl);
    SkllProtocol& set_v6only(bool enable = true);
    SkllProtocol& set_buffer_size(int size);
    SkllProtocol& set_chunk_size(int size);
    SkllProtocol& set_crlf(const std::string& crlf);
    SkllProtocol& set_timeout(int seconds, int keepidle = 0, int keepintvl = 0);
    
    // Getters
    int         get_fd() const;
    std::string get_name() const;
    std::string get_address() const;
    int         get_port() const;
    int         get_options() const;
    bool        get_bound() const;
    int         get_buffer_size() const;
    int         get_chunk_size() const;
    std::string get_crlf() const;
    int         get_timeout() const;
    bool        is_tcp() const;
    bool        is_udp() const;
    bool        is_ipv4() const;
    bool        is_ipv6() const;

private:
    std::string _get_socket_error();
    std::string _get_bind_error();
    std::string _get_listen_error();
};