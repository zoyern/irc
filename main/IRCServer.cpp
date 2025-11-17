/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 02:29:52 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 02:29:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServer.hpp"
#include <Sockell/SkllMessage.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>

IRCChannel::IRCChannel(const std::string& n)
    : name(n), modes(MODE_TOPIC_PROTECTED), user_limit(0) {}

IRCServer::IRCServer(const std::string& pass)
    : _cli_cnt(0), _chan_general("#general"), _chan_help("#help"), _pass(pass), _proto(NULL)
{
    _chan_help.modes |= MODE_READ_ONLY;
    _chan_help.topic = "Commands help - Read-only";
}

void IRCServer::set_protocol(SkllProtocol* p) {
    _proto = p;
}

// ═══════════════════════════════════════════════════
// HOOKS
// ═══════════════════════════════════════════════════

void IRCServer::on_connect(int fd) {
    if (_cli_cnt >= IRC_MAX_CLIENTS) {
        _send(fd, "ERROR :Server full");
        close(fd);
        return;
    }
    
    SkllClient* c = &_cli[_cli_cnt++];
    c->fd = fd;
    
    std::ostringstream o;
    o << "unknown_" << fd;
    c->id = o.str();
    c->buffer.clear();
    c->userdata = NULL;
    
    _fd_cli[fd] = c;
    if (_proto) _proto->add_client(fd, c);
    
    _send(c, ":server NOTICE * :*** Auth: PASS " + _pass);
    _send(c, ":server NOTICE * :*** Then: NICK <name> + USER <user> 0 * :<real>");
    
    std::cout << "[+] fd=" << fd << "\n";
}

void IRCServer::on_recv() {
    if (!_proto) return;
    
    const std::map<int, SkllClient*>& cls = _proto->get_clients();
    
    // Optimisation: itération directe
    for (std::map<int, SkllClient*>::const_iterator it = cls.begin(); it != cls.end(); ++it) {
        SkllClient* c = it->second;
        if (c->buffer.empty()) continue;
        
        SkllMessage msg(c->buffer);
        
        while (msg.has_line("\r\n", 2)) {
            SkllBuffer lb;
            if (msg.extract_line(lb, "\r\n", 2) == 0) break;
            
            std::string line(lb.begin(), lb.end());
            if (line.empty()) continue;
            
            std::string cmd, args;
            _parse(line, cmd, args);
            
            // Log compact
            std::string nick = (c->id.find("unknown_") == 0) ? "?" : c->id;
            std::cout << "[<] " << nick << ": " << cmd;
            if (!args.empty()) std::cout << " " << args;
            std::cout << "\n";
            
            // Dispatch optimisé (ordre fréquence)
            if (cmd == "PRIVMSG") {
                if (!_is_auth(c)) _send(c, ":server 464 * :Use PASS");
                else if (!_is_reg(c)) _send(c, ":server 451 * :Register first");
                else cmd_privmsg(c, args);
            }
            else if (cmd == "NICK") cmd_nick(c, args);
            else if (cmd == "PASS") cmd_pass(c, args);
            else if (cmd == "USER") cmd_user(c, args);
            else if (cmd == "JOIN") {
                if (!_is_auth(c)) _send(c, ":server 464 * :Use PASS");
                else if (!_is_reg(c)) _send(c, ":server 451 * :Register first");
                else cmd_join(c, args);
            }
            else if (cmd == "PING") cmd_ping(c, args);
            else if (cmd == "QUIT") cmd_quit(c, args);
            else if (cmd == "PART") {
                if (_is_auth(c) && _is_reg(c)) cmd_part(c, args);
            }
            else if (cmd == "KICK") {
                if (_is_auth(c) && _is_reg(c)) cmd_kick(c, args);
            }
            else if (cmd == "INVITE") {
                if (_is_auth(c) && _is_reg(c)) cmd_invite(c, args);
            }
            else if (cmd == "TOPIC") {
                if (_is_auth(c) && _is_reg(c)) cmd_topic(c, args);
            }
            else if (cmd == "MODE") {
                if (_is_auth(c) && _is_reg(c)) cmd_mode(c, args);
            }
            else if (cmd != "WHO" && cmd != "MOTD") {
                _send(c, ":server 421 " + c->id + " " + cmd + " :Unknown");
            }
        }
    }
}

void IRCServer::on_disconnect(int fd) {
    std::map<int, SkllClient*>::iterator it = _fd_cli.find(fd);
    if (it == _fd_cli.end()) return;
    
    SkllClient* c = it->second;
    
    // Cleanup (optimisé: une seule passe)
    if (c->id.find("unknown_") != 0) {
        _nick_cli.erase(c->id);
        _chan_general.clients.erase(c->id);
        _chan_general.operators.erase(c->id);
        _chan_help.clients.erase(c->id);
    }
    
    std::cout << "[-] " << c->id << "\n";
    
    _fd_cli.erase(it);
    if (_proto) _proto->remove_client(fd);
}

// ═══════════════════════════════════════════════════
// COMMANDS (ULTRA-COMPACT)
// ═══════════════════════════════════════════════════

void IRCServer::cmd_pass(SkllClient* c, const std::string& a) {
    if (a == _pass) {
        c->userdata = (void*)1;
        _send(c, ":server NOTICE * :Password OK");
    } else {
        _send(c, ":server 464 * :Bad password");
    }
}

void IRCServer::cmd_nick(SkllClient* c, const std::string& a) {
    std::string nick;
    
    // Parse nick (optimisé: break dès invalide)
    for (size_t i = 0; i < a.size(); ++i) {
        char x = a[i];
        if (x == ' ' || x == '\r' || x == '\n') break;
        if ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || 
            (x >= '0' && x <= '9') || x == '_' || x == '-') nick += x;
        else break;
    }
    
    if (nick.empty()) { _send(c, ":server 431 * :No nickname"); return; }
    
    // Check unicité (CRITIQUE: évite double nick)
    if (_nick_cli.find(nick) != _nick_cli.end()) {
        _send(c, ":server 433 * " + nick + " :In use");
        return;
    }
    
    // Update (optimisé: batch operations)
    if (c->id.find("unknown_") != 0) {
        _nick_cli.erase(c->id);
        
        // Update channels atomiquement
        if (_chan_general.clients.count(c->id)) {
            _chan_general.clients.erase(c->id);
            _chan_general.clients.insert(nick);
        }
        if (_chan_general.operators.count(c->id)) {
            _chan_general.operators.erase(c->id);
            _chan_general.operators.insert(nick);
        }
        if (_chan_help.clients.count(c->id)) {
            _chan_help.clients.erase(c->id);
            _chan_help.clients.insert(nick);
        }
    }
    
    std::string old = c->id;
    c->id = nick;
    _nick_cli[nick] = c;
    
    _send(c, ":" + old + " NICK " + nick);
    std::cout << "[IRC] fd=" << c->fd << " → " << nick << "\n";
}

void IRCServer::cmd_user(SkllClient* c, const std::string& a) {
    (void)a;
    if (c->id.find("unknown_") == 0) {
        _send(c, ":server 462 * :Set NICK first");
        return;
    }
    _welcome(c);
}

void IRCServer::cmd_join(SkllClient* c, const std::string& a) {
    if (a.empty()) { _send(c, ":server 461 " + c->id + " JOIN :Missing params"); return; }
    
    std::istringstream iss(a);
    std::string chan_name, key;
    iss >> chan_name >> key;
    
    IRCChannel* ch = _chan(chan_name);
    if (!ch) { _send(c, ":server 403 " + c->id + " " + chan_name + " :No such channel"); return; }
    
    // Check modes (compact)
    if ((ch->modes & MODE_INVITE_ONLY) && !ch->invited.count(c->id))
        { _send(c, ":server 473 " + c->id + " " + chan_name + " :Invite only"); return; }
    if ((ch->modes & MODE_KEY) && key != ch->key)
        { _send(c, ":server 475 " + c->id + " " + chan_name + " :Bad key"); return; }
    if ((ch->modes & MODE_USER_LIMIT) && (int)ch->clients.size() >= ch->user_limit)
        { _send(c, ":server 471 " + c->id + " " + chan_name + " :Full"); return; }
    
    ch->clients.insert(c->id);
    ch->invited.erase(c->id);
    
    // Premier = op (sauf #help)
    if (ch->clients.size() == 1 && chan_name != "#help") ch->operators.insert(c->id);
    
    _broadcast(ch, ":" + c->id + " JOIN " + chan_name, "");
    _send(c, ":server 332 " + c->id + " " + chan_name + " :" + ch->topic);
    
    // Userlist (optimisé: 1 passe)
    std::ostringstream u;
    u << ":server 353 " << c->id << " = " << chan_name << " :";
    for (std::set<std::string>::const_iterator it = ch->clients.begin(); it != ch->clients.end(); ++it) {
        if (ch->operators.count(*it)) u << "@";
        u << *it << " ";
    }
    _send(c, u.str());
    _send(c, ":server 366 " + c->id + " " + chan_name + " :End of NAMES");
    
    if (chan_name == "#help") _help(c);
    
    std::cout << "[IRC] " << c->id << " → " << chan_name << "\n";
}

void IRCServer::cmd_privmsg(SkllClient* c, const std::string& a) {
    size_t sp = a.find(' ');
    if (sp == std::string::npos) { _send(c, ":server 461 " + c->id + " PRIVMSG :Missing params"); return; }
    
    std::string tgt = a.substr(0, sp);
    std::string msg = a.substr(sp + 1);
    if (!msg.empty() && msg[0] == ':') msg = msg.substr(1);
    
    if (tgt[0] == '#') {
        IRCChannel* ch = _chan(tgt);
        if (!ch) { _send(c, ":server 403 " + c->id + " " + tgt + " :No such channel"); return; }
        if (!ch->clients.count(c->id)) { _send(c, ":server 442 " + c->id + " " + tgt + " :Not on channel"); return; }
        if (ch->modes & MODE_READ_ONLY) { _send(c, ":server 404 " + c->id + " " + tgt + " :Read-only"); return; }
        
        _broadcast(ch, ":" + c->id + " PRIVMSG " + tgt + " :" + msg, c->id);
        std::cout << "[IRC] " << c->id << " → " << tgt << ": " << msg << "\n";
    } else {
        std::map<std::string, SkllClient*>::iterator it = _nick_cli.find(tgt);
        if (it == _nick_cli.end()) { _send(c, ":server 401 " + c->id + " " + tgt + " :No such nick"); return; }
        
        _send(it->second, ":" + c->id + " PRIVMSG " + tgt + " :" + msg);
        std::cout << "[IRC] " << c->id << " → " + tgt + " (PM)\n";
    }
}

void IRCServer::cmd_kick(SkllClient* c, const std::string& a) {
    std::istringstream iss(a);
    std::string chan_name, tgt, reason;
    iss >> chan_name >> tgt;
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ':') reason = reason.substr(1);
    
    IRCChannel* ch = _chan(chan_name);
    if (!ch) { _send(c, ":server 403 " + c->id + " " + chan_name + " :No such channel"); return; }
    if (!_is_op(ch, c->id)) { _send(c, ":server 482 " + c->id + " " + chan_name + " :Not op"); return; }
    if (!ch->clients.count(tgt)) { _send(c, ":server 441 " + c->id + " " + tgt + " :Not on channel"); return; }
    
    ch->clients.erase(tgt);
    ch->operators.erase(tgt);
    
    _broadcast(ch, ":" + c->id + " KICK " + chan_name + " " + tgt + " :" + reason, "");
    std::cout << "[IRC] " << c->id << " kicked " << tgt << "\n";
}

void IRCServer::cmd_invite(SkllClient* c, const std::string& a) {
    std::istringstream iss(a);
    std::string tgt, chan_name;
    iss >> tgt >> chan_name;
    
    IRCChannel* ch = _chan(chan_name);
    if (!ch) { _send(c, ":server 403 " + c->id + " " + chan_name + " :No such channel"); return; }
    if (!_is_op(ch, c->id)) { _send(c, ":server 482 " + c->id + " " + chan_name + " :Not op"); return; }
    if (!_nick_cli.count(tgt)) { _send(c, ":server 401 " + c->id + " " + tgt + " :No such nick"); return; }
    
    ch->invited.insert(tgt);
    _send(c, ":server 341 " + c->id + " " + tgt + " " + chan_name);
    _send(_nick_cli[tgt], ":" + c->id + " INVITE " + tgt + " " + chan_name);
    
    std::cout << "[IRC] " << c->id << " invited " << tgt << "\n";
}

void IRCServer::cmd_topic(SkllClient* c, const std::string& a) {
    std::istringstream iss(a);
    std::string chan_name, topic;
    iss >> chan_name;
    std::getline(iss, topic);
    
    IRCChannel* ch = _chan(chan_name);
    if (!ch) { _send(c, ":server 403 " + c->id + " " + chan_name + " :No such channel"); return; }
    
    if (topic.empty()) { _send(c, ":server 332 " + c->id + " " + chan_name + " :" + ch->topic); return; }
    if ((ch->modes & MODE_TOPIC_PROTECTED) && !_is_op(ch, c->id)) { _send(c, ":server 482 " + c->id + " " + chan_name + " :Not op"); return; }
    
    if (!topic.empty() && topic[0] == ':') topic = topic.substr(1);
    ch->topic = topic;
    
    _broadcast(ch, ":" + c->id + " TOPIC " + chan_name + " :" + topic, "");
    std::cout << "[IRC] " << c->id << " set topic: " << topic << "\n";
}

void IRCServer::cmd_mode(SkllClient* c, const std::string& a) {
    std::istringstream iss(a);
    std::string chan_name, modes, param;
    iss >> chan_name >> modes >> param;
    
    IRCChannel* ch = _chan(chan_name);
    if (!ch) { _send(c, ":server 403 " + c->id + " " + chan_name + " :No such channel"); return; }
    if (!_is_op(ch, c->id)) { _send(c, ":server 482 " + c->id + " " + chan_name + " :Not op"); return; }
    
    bool add = true;
    for (size_t i = 0; i < modes.size(); ++i) {
        if (modes[i] == '+') add = true;
        else if (modes[i] == '-') add = false;
        else if (modes[i] == 'i') { if (add) ch->modes |= MODE_INVITE_ONLY; else ch->modes &= ~MODE_INVITE_ONLY; }
        else if (modes[i] == 't') { if (add) ch->modes |= MODE_TOPIC_PROTECTED; else ch->modes &= ~MODE_TOPIC_PROTECTED; }
        else if (modes[i] == 'k') { if (add) { ch->modes |= MODE_KEY; ch->key = param; } else { ch->modes &= ~MODE_KEY; ch->key = ""; } }
        else if (modes[i] == 'o') { if (add) ch->operators.insert(param); else ch->operators.erase(param); }
        else if (modes[i] == 'l') { if (add) { ch->modes |= MODE_USER_LIMIT; ch->user_limit = std::atoi(param.c_str()); } else { ch->modes &= ~MODE_USER_LIMIT; ch->user_limit = 0; } }
    }
    
    _broadcast(ch, ":" + c->id + " MODE " + chan_name + " " + modes + " " + param, "");
}

void IRCServer::cmd_part(SkllClient* c, const std::string& a) {
    std::istringstream iss(a);
    std::string chan_name;
    iss >> chan_name;
    
    IRCChannel* ch = _chan(chan_name);
    if (ch && ch->clients.count(c->id)) {
        ch->clients.erase(c->id);
        ch->operators.erase(c->id);
        _send(c, ":" + c->id + " PART " + chan_name);
        std::cout << "[IRC] " << c->id << " left " << chan_name << "\n";
    }
}

void IRCServer::cmd_quit(SkllClient* c, const std::string& a) {
    (void)a;
    _send(c, "ERROR :Closing");
    close(c->fd);
}

void IRCServer::cmd_ping(SkllClient* c, const std::string& a) {
    _send(c, "PONG :" + a);
}

// ═══════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════

void IRCServer::_welcome(SkllClient* c) {
    _send(c, ":server 001 " + c->id + " :Welcome!");
    _send(c, ":server 002 " + c->id + " :Host: ircserv v1.0");
    _send(c, ":server 004 " + c->id + " ircserv 1.0 o itkol");
    _send(c, ":server 375 " + c->id + " :- MOTD -");
    _send(c, ":server 372 " + c->id + " :- Channels: #general #help");
    _send(c, ":server 372 " + c->id + " :- /JOIN #help for commands");
    _send(c, ":server 376 " + c->id + " :End of MOTD");
}

void IRCServer::_help(SkllClient* c) {
    _send(c, ":server NOTICE " + c->id + " :===== COMMANDS =====");
    _send(c, ":server NOTICE " + c->id + " :PASS <password>");
    _send(c, ":server NOTICE " + c->id + " :NICK <name>");
    _send(c, ":server NOTICE " + c->id + " :USER <user> 0 * :<real>");
    _send(c, ":server NOTICE " + c->id + " :JOIN <#channel> [key]");
    _send(c, ":server NOTICE " + c->id + " :PRIVMSG <#chan|nick> :<msg>");
    _send(c, ":server NOTICE " + c->id + " :PART <#channel>");
    _send(c, ":server NOTICE " + c->id + " :");
    _send(c, ":server NOTICE " + c->id + " :===== OPERATORS =====");
    _send(c, ":server NOTICE " + c->id + " :KICK <#chan> <nick> :<reason>");
    _send(c, ":server NOTICE " + c->id + " :INVITE <nick> <#chan>");
    _send(c, ":server NOTICE " + c->id + " :TOPIC <#chan> :<topic>");
    _send(c, ":server NOTICE " + c->id + " :MODE <#chan> +i/t/k/o/l");
    _send(c, ":server NOTICE " + c->id + " :====================");
}

bool IRCServer::_is_auth(SkllClient* c) const { return c->userdata != NULL; }
bool IRCServer::_is_reg(SkllClient* c) const { return c->id.find("unknown_") != 0; }
bool IRCServer::_is_op(IRCChannel* ch, const std::string& n) const { return ch->operators.count(n) > 0; }

void IRCServer::_send(int fd, const std::string& m) {
    std::string full = m + "\r\n";
    send(fd, full.c_str(), full.size(), 0);
}

void IRCServer::_send(SkllClient* c, const std::string& m) {
    _send(c->fd, m);
}

void IRCServer::_parse(const std::string& line, std::string& cmd, std::string& args) {
    size_t st = 0;
    while (st < line.size() && line[st] == ' ') ++st;
    if (st < line.size() && line[st] == '/') ++st;
    
    size_t sp = line.find(' ', st);
    if (sp == std::string::npos) {
        cmd = line.substr(st);
        args = "";
    } else {
        cmd = line.substr(st, sp - st);
        args = line.substr(sp + 1);
    }
    
    // Uppercase (optimisé)
    for (size_t i = 0; i < cmd.size(); ++i)
        if (cmd[i] >= 'a' && cmd[i] <= 'z') cmd[i] = cmd[i] - 'a' + 'A';
}

IRCChannel* IRCServer::_chan(const std::string& n) {
    if (n == "#general") return &_chan_general;
    if (n == "#help") return &_chan_help;
    return NULL;
}

void IRCServer::_broadcast(IRCChannel* ch, const std::string& m, const std::string& ex) {
    // Optimisé: itération directe
    for (std::set<std::string>::const_iterator it = ch->clients.begin(); it != ch->clients.end(); ++it) {
        if (*it == ex) continue;
        
        std::map<std::string, SkllClient*>::iterator cit = _nick_cli.find(*it);
        if (cit != _nick_cli.end()) _send(cit->second, m);
    }
}