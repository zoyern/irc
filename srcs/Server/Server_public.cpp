/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_public.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 21:36:35 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 21:36:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

Server	&Server::port(const std::string& port) { _port = check_port(port); return (*this); }
Server	&Server::password(const std::string& pass) { _password = pass; return (*this); }
Server	&Server::name(const std::string& name) { _name = name; return (*this); }
Server	&Server::address(const std::string &address) { _address = address; return (*this); }
Server	&Server::connexion_msg(const std::string &msg) { _connexion_msg = msg; return (*this); }
Server	&Server::max_clients(int max) { _max_clients _init_limit(max); return (*this); }
Server	&Server::reserved_fds(int reserved) { _reserved_fds = _init_reserved(reserved); _init_limit(_max_clients - _reserved_fds);; return (*this); }
Server	&Server::timeout(int seconds) { _timeout = seconds; return (*this); }
Server	&Server::queue(int backlog) { _queue = backlog; return (*this); }
Server	&Server::hook(e_event event, Callback callback, void* data = NULL) { (void)event; (void)callback; (void)data; return (*this); }

Channel	&Server::channel(const std::string &name, bool is_default) {
	std::pair<std::map<std::string, Channel>::iterator, bool> channel =
    _channels.insert(std::pair<std::string, Channel>(name, Channel(name)));
	if (is_default) { debug(INFO, CHANNEL_SET_DEFAULT + name); _default_channel = channel.first->second; }
	channel.second ? debug(INFO, CHANNEL_CREATED + name) : debug(INFO, CHANNEL_EXISTS + name);
	return (channel.first->second);
}