/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 03:28:33 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 03:28:33 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell.hpp>

SkllServer::~SkllServer() {}
SkllServer::SkllServer(const std::string &port, const std::string &password)
	: errors(SkllErrors())
	, network(SkllNetwork())
	, protocol(SkllProtocol())
	, hooks(SkllHook())
	, console(SkllConsole::instance())
    , _fd(_init_socket())
	, _running(false)
	, _port(_check_port(port))
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
    console.log(INFO | WARNING, "SkllServer") << ":[ " << _name << " ] Initialized on port : [ " << _address << ":" << _port << " ]";
    console.log() << ":[max clients : " << _max_clients << " ]";
}

SkllServer::SkllServer(const SkllServer& other) : console(other.console) { *this = other; }

SkllServer&	SkllServer::operator=(const SkllServer& other) {
	if (this == &other) return (*this);
	return (
		_fd = other._fd,
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

int		SkllServer::run() { 
	return (console.info("SkllServer") << "Listening on " << _address << ":" << _port, 0);
}

void	SkllServer::stop() { _running = false; }

void	SkllServer::clean() {
	if (_fd >= 0) {
		close(_fd);
		_fd = -1;
	}
	console.info("SkllServer") << "SkllServer cleaned up.";
}


SkllChannel& SkllServer::channel(const std::string& name, bool is_default) {
    // TODO: implémenter correctement
    (void)name;
    (void)is_default;
    static Channel dummy("dummy");
    return (dummy);
}