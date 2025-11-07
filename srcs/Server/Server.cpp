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

Server::~Server() {}
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

Server::Server(const Server& other) { *this = other; }

Server&	Server::operator=(const Server& other) {
	if (this != &other) {
		_fd = other._fd;
		_epfd = other._epfd;
		_running = other._running;
		_port = other._port;
		_password = other._password;
		_name = other._name;
		_address = other._address;
		_connexion_msg = other._connexion_msg;
		_max_clients = other._max_clients;
		_reserved_fds = other._reserved_fds;
		_timeout = other._timeout;
		_queue = other._queue;
		_console = other._console;
		_default_channel = other._default_channel;
		_channels = other._channels;
		_clients = other._clients;
	}
	return (*this);
}


int		&Server::run() {}
void	&Server::stop() {}

