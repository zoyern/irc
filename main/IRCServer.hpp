/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 02:29:27 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 02:29:27 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllClient.hpp>
#include <map>
#include <set>
#include <string>

#define IRC_MAX_CLIENTS 1000

// Modes IRC
#define MODE_INVITE_ONLY     (1 << 0)
#define MODE_TOPIC_PROTECTED (1 << 1)
#define MODE_KEY             (1 << 2)
#define MODE_USER_LIMIT      (1 << 3)
#define MODE_READ_ONLY       (1 << 4)

// Channel IRC
struct IRCChannel {
    std::string name, topic, key;
    int modes, user_limit;
    std::set<std::string> clients, operators, invited;
    
    IRCChannel(const std::string& n);
};

// Serveur IRC
class IRCServer {
private:
    SkllClient  _cli[IRC_MAX_CLIENTS];
    size_t      _cli_cnt;
    
    std::map<int, SkllClient*>         _fd_cli;
    std::map<std::string, SkllClient*> _nick_cli;
    
    IRCChannel  _chan_general, _chan_help;
    std::string _pass;
    SkllProtocol* _proto;

public:
    IRCServer(const std::string& pass);
    
    void set_protocol(SkllProtocol* p);
    
    void on_connect(int fd);
    void on_recv();
    void on_disconnect(int fd);
    
    // Commands
    void cmd_pass(SkllClient* c, const std::string& a);
    void cmd_nick(SkllClient* c, const std::string& a);
    void cmd_user(SkllClient* c, const std::string& a);
    void cmd_join(SkllClient* c, const std::string& a);
    void cmd_privmsg(SkllClient* c, const std::string& a);
    void cmd_kick(SkllClient* c, const std::string& a);
    void cmd_invite(SkllClient* c, const std::string& a);
    void cmd_topic(SkllClient* c, const std::string& a);
    void cmd_mode(SkllClient* c, const std::string& a);
    void cmd_part(SkllClient* c, const std::string& a);
    void cmd_quit(SkllClient* c, const std::string& a);
    void cmd_ping(SkllClient* c, const std::string& a);

private:
    void _welcome(SkllClient* c);
    void _help(SkllClient* c);
    bool _is_auth(SkllClient* c) const;
    bool _is_reg(SkllClient* c) const;
    bool _is_op(IRCChannel* ch, const std::string& n) const;
    void _send(int fd, const std::string& m);
    void _send(SkllClient* c, const std::string& m);
    void _parse(const std::string& line, std::string& cmd, std::string& args);
    IRCChannel* _chan(const std::string& n);
    void _broadcast(IRCChannel* ch, const std::string& m, const std::string& ex = "");
};