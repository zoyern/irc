/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:13:52 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:13:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <IRC/IRCClient.hpp>
#include <IRC.hpp>
#include <Sockell/SkllEvent.hpp>
#include <sstream>

IRCClient::IRCClient()
	: skll_client(NULL), authenticated(false), registered(false)
	, password_attempts(0) {}

IRCClient::IRCClient(SkllClient *client)
	: skll_client(client), authenticated(false), registered(false)
	, password_attempts(0) {
	hostname = client->get_id();
	hook.on(SKLL_ON_CONNECT, on_auth, this);
	hook.on(SKLL_ON_RECV, on_message, this);
}

IRCClient::~IRCClient() {}

IRCClient &IRCClient::on(int event, SkllHook::Callback cb, void *user_data) {
    hook.on(event, cb, user_data);
    return *this;
}

void IRCClient::send(const std::string &msg) {
    if (skll_client) {
        skll_client->send(msg);
    }
}

void IRCClient::send_numeric(const std::string &code, const std::string &msg) {
    std::ostringstream oss;
    oss << ":server " << code << " " << nickname << " " << msg << "\r\n";
    send(oss.str());
}

void IRCClient::send_error(const std::string &code, const std::string &msg) {
    std::ostringstream oss;
    oss << ":server " << code << " " << (nickname.empty() ? "*" : nickname)
        << " " << msg << "\r\n";
    send(oss.str());
}

bool IRCClient::is_authenticated() const { return authenticated; }
bool IRCClient::is_registered() const { return registered; }

bool IRCClient::is_in_channel(const std::string &channel) const {
    return channels.find(channel) != channels.end();
}

void IRCClient::join_channel(const std::string &channel) {
    channels.insert(channel);
}

void IRCClient::part_channel(const std::string &channel) {
    channels.erase(channel);
}

int IRCClient::get_fd() const {
    return skll_client ? skll_client->get_fd() : -1;
}

const std::string &IRCClient::get_id() const {
    static std::string empty = "";
    return skll_client ? skll_client->get_id() : empty;
}

void IRCClient::on_auth(void *event, void *user_data) {
    (void)event;
    (void)user_data;
}

void IRCClient::on_register(void *event, void *user_data) {
    (void)event;
    (void)user_data;
}

void IRCClient::on_message(void *event, void *user_data) {
    (void)event;
    (void)user_data;
}
std::string IRCClient::get_prefix() const {
	std::string nick = nickname.empty() ? "*" : nickname;
	std::string user = username.empty() ? "user" : username;
	return ":" + nick + "!" + user + "@" + hostname;
}