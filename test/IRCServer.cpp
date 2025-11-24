/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:15:01 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:15:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <IRC/IRCServer.hpp>
#include <IRC/IRCErrors.hpp>
#include <IRC.hpp>
#include <Sockell/SkllEvent.hpp>
#include <iomanip>
#include <iostream>

IRCServer::IRCServer(const std::string &password)
    : _password(password), _name("irc") {

    _hook.on(SKLL_ON_START, on_server_start, this);
    _hook.on(SKLL_ON_CONNECT, on_client_connect, this);
    _hook.on(SKLL_ON_DISCONNECT, on_client_disconnect, this);

    setup_default_channels();
}

IRCServer::~IRCServer() {
    for (std::map<int, IRCClient*>::iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        delete it->second;
    }
    
    for (std::map<std::string, IRCChannel*>::iterator it = _channels.begin();
         it != _channels.end(); ++it) {
        delete it->second;
    }
}

IRCServer &IRCServer::on(int event, SkllHook::Callback cb, void *user_data) {
    _hook.on(event, cb, user_data);
    return *this;
}

void IRCServer::add_client(int fd, SkllClient *skll_client) {
    IRCClient *client = new IRCClient(skll_client);
    skll_client->userdata = client;
    _clients[fd] = client;
}

void IRCServer::remove_client(int fd) {
    std::map<int, IRCClient*>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        IRCClient *client = it->second;
        
        for (std::set<std::string>::iterator cit = client->channels.begin();
             cit != client->channels.end(); ++cit) {
            IRCChannel *chan = get_channel(*cit);
            if (chan && chan->skll_channel) {
                chan->skll_channel->remove_client(fd);
            }
        }
        
        delete client;
        _clients.erase(it);
    }
}

IRCClient *IRCServer::get_client(int fd) {
    std::map<int, IRCClient*>::iterator it = _clients.find(fd);
    return (it != _clients.end()) ? it->second : NULL;
}

IRCClient *IRCServer::get_client_by_nick(const std::string &nickname) {
    for (std::map<int, IRCClient*>::iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        if (it->second->nickname == nickname) {
            return it->second;
        }
    }
    return NULL;
}

IRCChannel *IRCServer::get_channel(const std::string &name) {
    std::map<std::string, IRCChannel*>::iterator it = _channels.find(name);
    return (it != _channels.end()) ? it->second : NULL;
}

IRCChannel *IRCServer::create_channel(const std::string &name) {
    IRCChannel *chan = get_channel(name);
    if (!chan) {
        chan = new IRCChannel(name);
        _channels[name] = chan;
    }
    return chan;
}

void IRCServer::remove_channel(const std::string &name) {
    std::map<std::string, IRCChannel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
    }
}

void IRCServer::setup_default_channels() {
    IRCChannel *general = create_channel("general");
    general->topic = "Welcome to the general discussion channel";

    IRCChannel *help = create_channel("help");
    help->read_only = true;
    help->topic = "Available commands - Read-only channel";
}

bool IRCServer::check_password(const std::string &password) const {
    return _password == password;
}

const std::string &IRCServer::get_name() const { return _name; }

void IRCServer::on_server_start(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event;
	IRCServer *irc = (IRCServer*)user_data;
	
	if (!evt->server) return;
	
	SkllServer *server = evt->server;

	std::cout << "\n"
			  << SKLL_CYAN << "╔════════════════════════════════════════╗\n"
			  << "║ " << SKLL_BOLD << "IRC SERVER" << SKLL_RESET << SKLL_CYAN 
			  << "                             \n"
			  << "╠════════════════════════════════════════\n"
			  << "║ " << SKLL_RESET << "Max Clients:  " 
			  << SKLL_BOLD << std::setw(4) << server->count() << SKLL_RESET << SKLL_CYAN
			  << "                      \n"
			  << "║ " << SKLL_RESET << "FD Reserved:  " 
			  << SKLL_BOLD << std::setw(4) << server->get_fd_limit() << SKLL_RESET << SKLL_CYAN
			  << "                      \n"
			  << "║ " << SKLL_RESET << "Password:     " 
			  << SKLL_DIM << (irc->_password.empty() ? "none" : "***") 
			  << SKLL_RESET << SKLL_CYAN << "                       \n"
			  << "╚════════════════════════════════════════╝"
			  << SKLL_RESET << "\n" << std::endl;
}

void IRCServer::on_client_connect(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event;
	IRCServer *irc = (IRCServer*)user_data;
	
	if (!evt->client) return;

	evt->client->set_rate_limit(10, 1);
	
	IRCClient *client = new IRCClient(evt->client);
	evt->client->userdata = client;
	
	irc->_clients[evt->client->get_fd()] = client;
	
	client->send(IRCErrors::welcome_banner());
	
	std::cout << SKLL_GREEN << "[CONNECT] " << SKLL_RESET 
			  << SKLL_BOLD << evt->client->get_id() << SKLL_RESET 
			  << " (fd=" << evt->client->get_fd() << ")" << std::endl;
}

void IRCServer::on_client_disconnect(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event;
	IRCServer *irc = (IRCServer*)user_data;
	
	if (!evt->client) return;
	
	IRCClient *client = (IRCClient*)evt->client->userdata;
	if (client) {
		std::ostringstream quit_msg;
		quit_msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
				 << " QUIT :Connection lost\r\n";
		
		std::set<std::string>::iterator it;
		for (it = client->channels.begin(); it != client->channels.end(); ++it) {
			IRCChannel *channel = irc->get_channel(*it);
			if (channel) {
				channel->broadcast(quit_msg.str(), client->get_fd());
			}
		}
		
		std::cout << SKLL_RED << "[DISCONNECT] " << SKLL_RESET;
		
		if (!client->nickname.empty()) {
			std::cout << SKLL_BOLD << client->nickname << SKLL_RESET << " ";
		}
		
		std::cout << "(" << evt->client->get_id() << ")" << std::endl;
	}
	
	irc->remove_client(evt->client->get_fd());
}