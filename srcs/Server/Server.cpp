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
Server::Server(const std::string &port, const std::string &password)
	: console(Console::instance())
    , _fd(_init_socket())
	, _running(false)
	, _epoll(_init_epoll())
	, _port(check_port(port))
	, _password(password)
	, _name(NAME)
	, _address(ADDRESS)
	, _connexion_msg(MSG)
	, _max_clients(_init_limit(MAX_CLIENTS))
	, _reserved_fds(_init_reserved(RESERVED_FD))
	, _timeout(TIMEOUT)
	, _queue(QUEUE)
	, _default_channel(NULL)
{
    console.log(INFO | WARNING, "Server") << ":[ " << _name << " ] Initialized on port : [ " << _address << ":" << _port << " ]";
    console.log() << ":[max clients : " << _max_clients << " ]";
}

Server::Server(const Server& other) : console(other.console) { *this = other; }

Server&	Server::operator=(const Server& other) {
	if (this == &other) return (*this);
	return (
		_fd = other._fd,
		_epoll = other._epoll,
		_running = other._running,
		_port = other._port,
		_password = other._password,
		_name = other._name,
		_address = other._address,
		_connexion_msg = other._connexion_msg,
		_max_clients = other._max_clients,
		_reserved_fds = other._reserved_fds,
		_timeout = other._timeout,
		_queue = other._queue,
		_default_channel = other._default_channel,
		_channels = other._channels,
		_clients = other._clients,
		*this);
}

int		Server::run() { 
	console.info("Server") << "Listening on " << _address << ":" << _port;

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	fcntl(_fd, F_SETFL, O_NONBLOCK);

	if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		return (clean(), throw std::runtime_error("bind() failed"), 0);
	if (listen(_fd, _queue) < 0)
		return (clean(), throw std::runtime_error("listen() failed"), 0);

	_epoll.add(_fd, EPOLLIN);

	console.info("Server") << "Server started.";

	_running = true;
	while (_running) {
		int n = _epoll.wait(_timeout);
		for (int i = 0; i < n; i++) {
			struct epoll_event *ev = &_epoll.events()[i];
			if (ev->data.fd == _fd) {
				int cli = accept(_fd, NULL, NULL);
				if (cli < 0)
					continue;
				fcntl(cli, F_SETFL, O_NONBLOCK);
				_epoll.add(cli, EPOLLIN);
				console.info("Server") << "New client fd=" << cli;
			} else {
				char buf[512];
				int bytes = recv(ev->data.fd, buf, 512, 0);
				if (bytes <= 0) {
					console.info("Server") << "Client disconnected fd=" << ev->data.fd;
					_epoll.del(ev->data.fd);
					close(ev->data.fd);
				} else {
					buf[bytes] = '\0';
					console.debug("Server") << "[" << ev->data.fd << "] " << buf;
					send(ev->data.fd, buf, bytes, 0);
				}
			}
		}
	}
	return (0);
}

void	Server::stop() { _running = false; }

void	Server::clean() {
	if (_fd >= 0) {
		close(_fd);
		_fd = -1;
	}
	console.info("Server") << "Server cleaned up.";
}