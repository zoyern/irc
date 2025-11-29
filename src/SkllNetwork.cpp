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
#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllException.hpp>
#include <Sockell/SkllLog.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

/* Constructors with dynamic event buffer */
SkllNetwork::SkllNetwork()
	: _server(0), _timeout(-1), _delim("\r\n"), _epoll_fd(-1),
	  _max_events(SKLL_DEFAULT_MAX_EVENTS), _events(SKLL_DEFAULT_MAX_EVENTS),
	  _conn_limit(0), _conn_window(SKLL_DEFAULT_CONN_WINDOW),
	  _msg_limit(0), _msg_window(SKLL_DEFAULT_MSG_WINDOW) {}

SkllNetwork::SkllNetwork(const std::string &name)
	: _server(0), _name(name), _timeout(-1), _delim("\r\n"), _epoll_fd(-1),
	  _max_events(SKLL_DEFAULT_MAX_EVENTS), _events(SKLL_DEFAULT_MAX_EVENTS),
	  _conn_limit(0), _conn_window(SKLL_DEFAULT_CONN_WINDOW),
	  _msg_limit(0), _msg_window(SKLL_DEFAULT_MSG_WINDOW) {}

SkllNetwork::~SkllNetwork() {
	/* Don't iterate over _protocols - they may already be destroyed */
	/* Close listening fds directly (we have the fd numbers) */
	for (std::map<fd_t, SkllProtocol *>::iterator it = _listen_fds.begin(); it != _listen_fds.end(); ++it)
		::close(it->first);
	/* Close client fds */
	for (std::map<fd_t, SkllClient>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		::close(it->first);
	/* Close epoll fd */
	if (_epoll_fd >= 0) ::close(_epoll_fd);
}

SkllNetwork &SkllNetwork::set_name(const std::string &name) { _name = name; return *this; }
SkllNetwork &SkllNetwork::set_timeout(int ms) { _timeout = ms; return *this; }
SkllNetwork &SkllNetwork::set_delim(const std::string &d) { _delim = d; return *this; }

SkllNetwork &SkllNetwork::set_max_events(size_t n) {
	_max_events = (n > 0) ? n : 1;
	_events.resize(_max_events);
	return *this;
}

void	SkllNetwork::set_server(SkllServer *s) { _server = s; }
void	SkllNetwork::disconnect(SkllEvent &e){ e.channels().remove_all(e.fd()); _handle_close(e.fd());}
const std::string &SkllNetwork::name() const { return _name; }
int SkllNetwork::timeout() const { return _timeout; }
const std::string &SkllNetwork::delim() const { return _delim; }
size_t SkllNetwork::max_events() const { return _max_events; }
fd_t SkllNetwork::epoll_fd() const { return _epoll_fd; }
SkllServer *SkllNetwork::server() { return _server; }

SkllNetwork &SkllNetwork::add(SkllProtocol &proto) {
	_protocols.push_back(&proto);
	return *this;
}

size_t SkllNetwork::protocol_count() const { return _protocols.size(); }

SkllClient &SkllNetwork::add_client(fd_t fd) {
	_clients[fd].init(fd);
	return _clients[fd];
}

SkllClient *SkllNetwork::get_client(fd_t fd) {
	std::map<fd_t, SkllClient>::iterator it = _clients.find(fd);
	return (it != _clients.end()) ? &it->second : 0;
}

void SkllNetwork::remove_client(fd_t fd) {
	_clients.erase(fd);
}

size_t SkllNetwork::client_count() const { return _clients.size(); }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   RATE LIMITING                                                             */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllNetwork &SkllNetwork::set_conn_limit(size_t max_per_ip, int window_sec) {
	_conn_limit = max_per_ip;
	_conn_window = window_sec;
	return *this;
}

SkllNetwork &SkllNetwork::set_msg_limit(size_t max_per_client, int window_sec) {
	_msg_limit = max_per_client;
	_msg_window = window_sec;
	return *this;
}

bool SkllNetwork::check_conn_rate(const std::string &ip) {
	if (_conn_limit == 0) return true; /* Disabled */
	
	time_t now = std::time(0);
	SkllRateLimit &rate = _conn_rates[ip];
	
	/* Reset window if expired */
	if (now - rate.window_start >= _conn_window) {
		rate.count = 0;
		rate.window_start = now;
	}
	
	rate.count++;
	return rate.count <= _conn_limit;
}

bool SkllNetwork::check_msg_rate(fd_t fd) {
	if (_msg_limit == 0) return true; /* Disabled */
	
	time_t now = std::time(0);
	SkllRateLimit &rate = _msg_rates[fd];
	
	/* Reset window if expired */
	if (now - rate.window_start >= _msg_window) {
		rate.count = 0;
		rate.window_start = now;
	}
	
	rate.count++;
	return rate.count <= _msg_limit;
}

void SkllNetwork::clear_rate_limits() {
	_conn_rates.clear();
	_msg_rates.clear();
}

SkllNetwork &SkllNetwork::on(int type, SkllCallback fn) {
	if (type &SKLL_EV_ACCEPT)  _on_accept.add(fn);
	if (type &SKLL_EV_CLOSE)   _on_close.add(fn);
	if (type &SKLL_EV_MESSAGE) _on_message.add(fn);
	if (type &SKLL_EV_ERROR)   _on_error.add(fn);
	return *this;
}

SkllHooks &SkllNetwork::hooks(int type) {
	if (type &SKLL_EV_ACCEPT)  return _on_accept;
	if (type &SKLL_EV_CLOSE)   return _on_close;
	if (type &SKLL_EV_ERROR)   return _on_error;
	return _on_message;
}

SkllRouter &SkllNetwork::router() { return _router; }

bool SkllNetwork::init() {
	if (_protocols.empty())
		throw SkllConfigException();
	
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd < 0)
		throw SkllEpollException();
	
	for (size_t i = 0; i < _protocols.size(); ++i) {
		_protocols[i]->bind(); /* Throws on error */
		
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = _protocols[i]->fd();
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _protocols[i]->fd(), &ev) < 0)
			throw SkllEpollException();
		_listen_fds[_protocols[i]->fd()] = _protocols[i];
	}
	
	std::ostringstream oss;
	oss << _name << " network initialized";
	SkllLog::info(oss.str());
	return true;
}

void SkllNetwork::close() {
	/* Close all client connections */
	for (std::map<fd_t, SkllClient>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		::close(it->first);
	_clients.clear();
	
	/* Close listening protocols - check if not already closed */
	for (size_t i = 0; i < _protocols.size(); ++i) {
		if (_protocols[i]) _protocols[i]->close();
	}
	_protocols.clear();  /* Prevent double-close */
	_listen_fds.clear();
	
	/* Close epoll fd */
	if (_epoll_fd >= 0) {
		::close(_epoll_fd);
		_epoll_fd = -1;
	}
}

int SkllNetwork::poll_once() {
	/* Use dynamic event buffer - no fixed limit */
	int n = epoll_wait(_epoll_fd, &_events[0], static_cast<int>(_max_events), _timeout);
	
	for (int i = 0; i < n; ++i) {
		fd_t fd = _events[i].data.fd;
		
		std::map<fd_t, SkllProtocol *>::iterator lit = _listen_fds.find(fd);
		if (lit != _listen_fds.end()) {
				_handle_accept(*lit->second);
			continue;
		}
		
		if (_events[i].events &(EPOLLHUP | EPOLLERR)) {
			_handle_close(fd);
		} else if (_events[i].events &EPOLLIN) {
			_handle_read(fd);
		}
	}
	
	return n;
}

void SkllNetwork::_handle_accept(SkllProtocol &proto) {
	/* Use sockaddr_storage to support both IPv4 and IPv6 */
	struct sockaddr_storage addr_storage;
	socklen_t len = sizeof(addr_storage);
	fd_t cfd = accept(proto.fd(), reinterpret_cast<struct sockaddr *>(&addr_storage), &len);
	if (cfd < 0) return;
	
	/* Extract IP address - handle both IPv4 and IPv6 */
	char ip_buf[64];
	std::string ip;
	int port = 0;
	
	if (addr_storage.ss_family == AF_INET) {
		struct sockaddr_in *addr4 = reinterpret_cast<struct sockaddr_in *>(&addr_storage);
		inet_ntop(AF_INET, &addr4->sin_addr, ip_buf, sizeof(ip_buf));
		port = ntohs(addr4->sin_port);
	} else if (addr_storage.ss_family == AF_INET6) {
		struct sockaddr_in6 *addr6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_storage);
		inet_ntop(AF_INET6, &addr6->sin6_addr, ip_buf, sizeof(ip_buf));
		port = ntohs(addr6->sin6_port);
	}
	ip = ip_buf;
	
	/* Rate limit check - reject if too many connections from this IP */
	if (!check_conn_rate(ip)) {
		SkllLog::warn("Rate limit exceeded for " + ip + " - connection rejected");
		::close(cfd);
		return;
	}
	
	fcntl(cfd, F_SETFL, O_NONBLOCK);
	
	if (proto.nodelay()) {
		int opt = 1;
		setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	}
	if (proto.keepalive()) {
		int opt = 1;
		setsockopt(cfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
	}
	
	SkllClient &client = add_client(cfd);
	client.set_addr(ip, port);
	
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
	ev.data.fd = cfd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, cfd, &ev);
	
	std::ostringstream oss;
	oss << "Client " << cfd << " (" << client.ip() << ") connected";
	SkllLog::info(oss.str());
	
	SkllEvent event;
	event.set_event_type(SKLL_EV_ACCEPT);
	event.set_fd(cfd);
	event.set_server(_server);
	event.set_network(this);
	event.set_client(&client);
	_on_accept.run(event);
}

void SkllNetwork::_handle_read(fd_t fd) {
	SkllClient *client = get_client(fd);
	if (!client) return;
	
	byte_t buf[4096];
	ssize_t n = recv(fd, buf, sizeof(buf), 0);
	
	if (n <= 0) {
		_handle_close(fd);
		return;
	}
	
	client->in().write(buf, n);
	
	SkllSpan line;
	while (!(line = client->in().line(_delim.c_str())).empty()) {
		/* Rate limit check - ignore if flooding */
		if (!check_msg_rate(fd)) {
			SkllLog::warn("Message rate limit exceeded - ignoring");
			continue;
		}
		
		SkllEvent event;
		event.set_event_type(SKLL_EV_MESSAGE);
		event.set_fd(fd);
		event.set_server(_server);
		event.set_network(this);
		event.set_client(client);
		event.message().set(line);
		
		int ret = _router.dispatch(event);
		if (ret == SKLL_FATAL) return;
		
		/* SKLL_STOP means disconnect this client */
		if (ret == SKLL_STOP) {
			_handle_close(fd);
			return;
		}
		
		if (ret != SKLL_SKIP)
			_on_message.run(event);
	}
}

void SkllNetwork::_handle_close(fd_t fd) {
	SkllClient *client = get_client(fd);
	
	SkllEvent event;
	event.set_event_type(SKLL_EV_CLOSE);
	event.set_fd(fd);
	event.set_server(_server);
	event.set_network(this);
	event.set_client(client);
	_on_close.run(event);
	
	if (client) {
		std::ostringstream oss;
		oss << "Client " << fd << " (" << client->ip() << ") disconnected";
		SkllLog::info(oss.str());
	}
	
	/* Clean up rate limit entry */
	_msg_rates.erase(fd);
	
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0);
	::close(fd);
	remove_client(fd);
}

std::map<fd_t, SkllClient> &SkllNetwork::clients() { return _clients; }
const std::map<fd_t, SkllClient> &SkllNetwork::clients() const { return _clients; }

