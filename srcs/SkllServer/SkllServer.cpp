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

#include <Sockell/SkllServer.hpp>
//port, ADDRESS, MAX_CLIENTS, RESERVED_FD, TIMEOUT, QUEUE
SkllServer::~SkllServer() {}
SkllServer::SkllServer(const std::string &port, const std::string &password, const std::string &name, const std::string &msg)
	: errors(SkllErrors())
	, network(SkllNetwork(port))
	, hooks(SkllHook())
	, console(SkllConsole::instance())
    , _fd(0)
	, _running(false)
	, _password(password)
	, _name(name)
	, _connexion_msg(msg)
	, _default_channel(NULL)
{
    console.log(INFO | WARNING, "SkllServer") << ":[ " << _name << " ] Initialized on port : [ " << network.get_address() << ":" << network.get_port() << " ]";
    console.log() << ":[max clients : " << network.get_max_client() << " ]";
}

SkllServer::SkllServer(const SkllServer& other) 
	: errors(other.errors)
	, network(other.network)
	, hooks(other.hooks)
	, console(other.console)
{ *this = other; }

SkllServer&	SkllServer::operator=(const SkllServer& other) {
	if (this == &other) return (*this);
	return (
		_fd = other._fd,
		_running = other._running,
		_password = other._password,
		_name = other._name,
		_connexion_msg = other._connexion_msg,
		_default_channel = other._default_channel,
		_channels = other._channels,
		_clients = other._clients,
		*this);
}

int		SkllServer::run() { 
	return (console.info("SkllServer") << "Listening on " << network.get_address() << ":" << network.get_port(), 0);
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
    static SkllChannel dummy("dummy");
    return (dummy);
}