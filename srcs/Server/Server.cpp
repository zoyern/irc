/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 03:28:33 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 03:28:33 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

Server::Server(const std::string &port, const std::string &password, bool console = false, std::string path = "irc.log") 
    : fd(init_socket())
	,_epfd(_init_epoll()),
	_running(false),
	_port(check_port(port)),
	_password(password),
	_name(NAME),
	_address(ADDRESS),
	_connexion_msg(MSG),
	_max_clients(_init_max_clients(MAX_CLIENTS)),
	_reserved_fds(RESERVED_FD),
	_used_reserved_fds(0),
	_timeout(TIMEOUT),
	_queue(QUEUE),
	_default_channel(NULL)
{
	console.log(INFO | WARNING, ":[ " + _name + " ]" + " Initialized on port : [ " + _address + _port + "]", "Server::Server");
	console.log(":[max clients : "  + _max_clients + " ]", "Server::Server");
}

void Server::_update_reserved_fds(int delta) { _used_reserved_fds += delta;	_max_clients = _calculate_max_clients(MAX_CLIENTS, _reserved_fds); }

uint16_t Server::check_port(const std::string &port) {
	std::istringstream iss(port);
	int port_num;
	if (!(iss >> port_num) || port_num > 65535 || port_num < 1)
		throw (std::invalid_argument("Invalid port"));
	return(static_cast<uint16_t>(port_num));
}

Channel &Server::channel(const std::string &name, bool is_default) {
	std::pair<std::map<std::string, Channel>::iterator, bool> channel =
    _channels.insert(std::pair<std::string, Channel>(name, Channel(name)));
	if (is_default) { debug(INFO, CHANNEL_SET_DEFAULT + name); _default_channel = channel.first->second; }
	channel.second ? debug(INFO, CHANNEL_CREATED + name) : debug(INFO, CHANNEL_EXISTS + name);
	return (channel.first->second);
}
