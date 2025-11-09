/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllNetwork.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 18:53:28 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 18:53:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllNetwork.hpp>

SkllNetwork::~SkllNetwork() {}
SkllNetwork::SkllNetwork(const std::string &port)
	: errors(SkllErrors())
	, hooks(SkllHook())
	, console(SkllConsole::instance())
    , _epfd(0)
	, _max_clients(SKLL_MAX_CLIENTS)
	, _reserved_fds(SKLL_RESERVED_FD)
	, _timeout(SKLL_TIMEOUT)
	, _queue(SKLL_QUEUE)
	, _running(false)
	, _port(_check_port(port))
	, _address(SKLL_ADDRESS)
{
    console.log(INFO | WARNING, "SkllNetwork") << ":[ " << get_address() << " ] Initialized on port : [ " << get_address() << ":" << get_port() << " ]";
    console.log() << ":[max clients : " << get_max_client() << " ]";
}

SkllNetwork::SkllNetwork(const SkllNetwork& other) : console(other.console) { *this = other; }

SkllNetwork&	SkllNetwork::operator=(const SkllNetwork& other) {
	if (this == &other) return (*this);
	return (
		_epfd = other._epfd,
		_max_clients = other._max_clients,
		_reserved_fds = other._reserved_fds,
		_timeout = other._timeout,
		_queue = other._queue,
		_running = other._running,
		_port = other._port,
		_address = other._address,
		*this);
}

int		SkllNetwork::run() { return (console.info("SkllNetwork") << "Listening on " << get_address() << ":" << get_port(), 0); }
void	SkllNetwork::stop() { _running = false; }
void	SkllNetwork::clean() {
	if (_epfd >= 0) { close(_epfd); _epfd = -1; }
	console.info("SkllNetwork") << "SkllNetwork cleaned up.";
}

// ================================
// Setters
// ================================
SkllNetwork	&SkllNetwork::set_address(const std::string &address) { _address = address; return (*this); }
SkllNetwork	&SkllNetwork::set_port(const std::string& port) { _port = _check_port(port); return (*this); }
SkllNetwork	&SkllNetwork::set_max_clients(int max) { _max_clients = _init_limit(max); return (*this); }
SkllNetwork	&SkllNetwork::set_reserved_fds(int reserved) { _reserved_fds = _init_reserved(reserved); _init_limit(_max_clients - _reserved_fds); return (*this); }
SkllNetwork	&SkllNetwork::set_timeout(int seconds) { _timeout = seconds; return (*this); }
SkllNetwork	&SkllNetwork::set_queue(int backlog) { _queue = backlog; return (*this); }
SkllNetwork	&SkllNetwork::set_protocol(const std::string &crlf, bool binary, int protocol, int buffer_size) { (void)crlf;(void)buffer_size;(void)binary;(void)protocol; return (*this);}

// ================================
// Getters
// ================================
std::string &SkllNetwork::get_address() { return (_address);}
uint16_t	&SkllNetwork::get_port() { return (_port);}
int			&SkllNetwork::get_max_client() { return (_max_clients);}
int			&SkllNetwork::get_reserved_fds() { return (_reserved_fds);}
int			&SkllNetwork::get_timeout() { return (_timeout);}
int			&SkllNetwork::get_queue() { return (_queue);}
