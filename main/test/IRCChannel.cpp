/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannel.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:14:40 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:14:40 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <IRC/IRCChannel.hpp>
#include <IRC/IRCClient.hpp>
#include <IRC.hpp>
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllEvent.hpp>
#include <sstream>
#include <iostream>

IRCChannel::IRCChannel()
    : skll_channel(NULL), user_limit(0)
    , invite_only(false), topic_protected(false), read_only(false) {}

IRCChannel::IRCChannel(const std::string &name)
    : user_limit(0), invite_only(false), topic_protected(false), read_only(false) {
    skll_channel = new SkllChannel(name);
    
    skll_channel->on(SKLL_ON_CONNECT, on_client_join, this);
    skll_channel->on(SKLL_ON_DISCONNECT, on_client_part, this);
    skll_channel->on(SKLL_ON_RECV, on_client_message, this);
}

IRCChannel::~IRCChannel() {
    delete skll_channel;
}

IRCChannel &IRCChannel::on(int event, SkllHook::Callback cb, void *user_data) {
    hook.on(event, cb, user_data);
    return *this;
}

void IRCChannel::broadcast(const std::string &msg, int exclude_fd) {
    if (!skll_channel) return;
    skll_channel->broadcast(msg, exclude_fd);
}

void IRCChannel::send_topic(int fd) {
    if (!skll_channel) return;
    
    SkllClient *client = skll_channel->get_client(fd);
    if (!client) return;
    
    std::ostringstream oss;
    if (topic.empty()) {
        oss << ":server " << RPL_NOTOPIC << " * #" << skll_channel->name
            << " :No topic is set\r\n";
    } else {
        oss << ":server " << RPL_TOPIC << " * #" << skll_channel->name
            << " :" << topic << "\r\n";
    }
    client->send(oss.str());
}

void IRCChannel::send_names(int fd) {
    if (!skll_channel) return;
    
    SkllClient *client = skll_channel->get_client(fd);
    if (!client) return;
    
    std::ostringstream oss;
    oss << ":server " << RPL_NAMREPLY << " * = #" << skll_channel->name << " :";
    
    for (std::map<int, SkllClient*>::iterator it = skll_channel->clients.begin();
         it != skll_channel->clients.end(); ++it) {
        if (is_operator(it->first)) {
            oss << "@";
        }
        
        IRCClient *irc_client = (IRCClient*)it->second->userdata;
        if (irc_client) {
            oss << irc_client->nickname << " ";
        }
    }
    oss << "\r\n";
    
    client->send(oss.str());
    
    oss.str("");
    oss << ":server " << RPL_ENDOFNAMES << " * #" << skll_channel->name
        << " :End of /NAMES list\r\n";
    client->send(oss.str());
}

bool IRCChannel::is_operator(int fd) const {
    return operators.find(fd) != operators.end();
}

void IRCChannel::set_operator(int fd, bool op) {
    if (op) {
        operators.insert(fd);
    } else {
        operators.erase(fd);
    }
}

bool IRCChannel::is_invited(int fd) const {
    return invited.find(fd) != invited.end();
}

void IRCChannel::invite(int fd) {
    invited.insert(fd);
}

bool IRCChannel::has_password() const {
    return !password.empty();
}

bool IRCChannel::check_password(const std::string &pass) const {
    return password == pass;
}

bool IRCChannel::is_full() const {
    if (user_limit <= 0) return false;
    return skll_channel &&(int)skll_channel->client_count() >= user_limit;
}

bool IRCChannel::can_join(int fd, const std::string &key) const {
    if (is_full()) return false;
    if (invite_only &&!is_invited(fd)) return false;
    if (has_password() && !check_password(key)) return false;
    return true;
}

bool IRCChannel::can_send(int fd) const {
    if (read_only &&!is_operator(fd)) return false;
    return true;
}

const std::string &IRCChannel::get_name() const {
    static std::string empty = "";
    return skll_channel ? skll_channel->name : empty;
}

size_t IRCChannel::get_client_count() const {
    return skll_channel ? skll_channel->client_count() : 0;
}

void IRCChannel::on_client_join(void *event_ptr, void *channel_ptr) {
	SkllEvent *event = (SkllEvent*)event_ptr;
	IRCChannel *channel = (IRCChannel*)channel_ptr;
	
	if (!event || !channel || !event->client || !event->client->userdata) return;
	
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	irc_log("JOIN", client, "#" + channel->get_name());
}

void IRCChannel::on_client_part(void *event_ptr, void *channel_ptr) {
	SkllEvent *event = (SkllEvent*)event_ptr;
	IRCChannel *channel = (IRCChannel*)channel_ptr;
	
	if (!event || !channel || !event->client || !event->client->userdata) return;
	
	IRCClient *client = (IRCClient*)event->client->userdata;

	irc_log("PART", client, "#" + channel->get_name());
}

void IRCChannel::on_client_message(void *event, void *user_data) {
    SkllEvent *evt = (SkllEvent*)event;
    IRCChannel *channel = (IRCChannel*)user_data;
    
    if (!evt->client || !channel) return;
    
    IRCClient *client = (IRCClient*)evt->client->userdata;
    if (!client) return;

    if (channel->read_only &&!channel->is_operator(client->get_fd())) {
        client->send_error(ERR_CANNOTSENDTOCHAN, 
                          "#" + channel->get_name() + " :Cannot send to channel (read-only)");
        return;
    }
}