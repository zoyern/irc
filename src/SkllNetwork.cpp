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
#include <Sockell/SkllErrors.hpp>
#include <Sockell.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

SkllNetwork::SkllNetwork()
	: _epfd(-1), _signal_fd(-1), _name("default"), _timeout(100), _queue(128)
	, _events(EPOLLIN | EPOLLRDHUP | EPOLLET)
	, _started(false), _running(false), _stopped(false)
	, _server(NULL) {}

SkllNetwork::SkllNetwork(const std::string &name, int timeout, int queue)
	: _epfd(-1), _signal_fd(-1), _name(name), _timeout(timeout), _queue(queue)
	, _events(EPOLLIN | EPOLLRDHUP | EPOLLET)
	, _started(false), _running(false), _stopped(false)
	, _server(NULL) {}

SkllNetwork::~SkllNetwork() {
	shutdown();
}

int SkllNetwork::start() {
	if (_started) return 0;
	
	_epfd = epoll_create1(EPOLL_CLOEXEC);
	if (_epfd < 0) {
		throw SkllErrorEpoll("epoll_create1() failed");
	}
	
	_ev.resize(_queue);
	setup_signals();
	
	_started = true;
	trigger_event(SKLL_ON_START);
	
	return 0;
}

int SkllNetwork::run() {
	if (!_started) start();
	if (_running) return 0;
	
	for (std::map<std::string, SkllProtocol*>::iterator it = _protocols.begin();
		 it != _protocols.end(); ++it) {
		
		SkllProtocol *proto = it->second;
		proto->run();
		
		_fd_to_protocol[proto->get_fd()] = proto;
		add_to_epoll(proto->get_fd(), _events);
	}
	
	_running = true;
	
	print_startup_info();
	
	return 0;
}

void SkllNetwork::print_startup_info() {
	if (_protocols.empty()) return;
	
	std::cout << SKLL_CYAN << "┌────────────────────────────────────────┐\n"
			  << "│ " << SKLL_BOLD << _name << SKLL_RESET << SKLL_CYAN;
	
	int name_len = _name.length();
	for (int i = 0; i < (38 - name_len); ++i) std::cout << " ";
	std::cout << "\n"
			  << "├────────────────────────────────────────┤\n";
	
	for (std::map<std::string, SkllProtocol*>::iterator it = _protocols.begin();
		 it != _protocols.end(); ++it) {
		
		SkllProtocol *proto = it->second;
		
		std::cout << "│ " << SKLL_GREEN << "●" << SKLL_RESET << " "
				  << std::left << std::setw(8) << proto->get_name()
				  << SKLL_DIM << proto->get_address() << ":" << proto->get_port()
				  << SKLL_RESET << SKLL_CYAN;
		
		std::ostringstream addr;
		addr << proto->get_address() << ":" << proto->get_port();
		int addr_len = proto->get_name().length() + addr.str().length();
		for (int i = 0; i < (27 - addr_len); ++i) std::cout << " ";
		std::cout << "\n";
	}
	
	std::cout << "├────────────────────────────────────────┤\n"
			  << "│ " << SKLL_DIM << "Epoll: " << _epfd 
			  << "  │  Signal: " << SkllSignals::get_fd()
			  << SKLL_RESET << SKLL_CYAN << "\n"
			  << "└────────────────────────────────────────┘"
			  << SKLL_RESET << std::endl;
}

const std::map<std::string, SkllProtocol*> &SkllNetwork::get_protocols() const {
	return _protocols;
}

int SkllNetwork::stop() {
	if (!_running || _stopped) return 0;
	
	_stopped = true;
	_running = false;
	
	trigger_event(SKLL_ON_STOP);
	
	return 0;
}

int SkllNetwork::shutdown() {
	if (_epfd < 0) return 0;
	
	for (std::map<std::string, SkllProtocol*>::iterator it = _protocols.begin();
		 it != _protocols.end(); ++it) {
		it->second->stop();
	}
	
	for (std::map<std::string, SkllProtocol*>::iterator it = _protocols.begin();
		 it != _protocols.end(); ++it) {
		
		const std::map<int, SkllClient*> &clients = it->second->get_clients();
		
		std::vector<int> fds_to_close;
		for (std::map<int, SkllClient*>::const_iterator cit = clients.begin();
			 cit != clients.end(); ++cit) {
			fds_to_close.push_back(cit->first);
		}
		
		for (size_t i = 0; i < fds_to_close.size(); ++i) {
			SkllClient *client = it->second->get_client(fds_to_close[i]);
			if (client) {
				it->second->remove_client(fds_to_close[i]);
				close(fds_to_close[i]);
				delete client;
			}
		}
	}
	
	if (_signal_fd >= 0) {
		remove_from_epoll(_signal_fd);
		SkllSignals::cleanup();
		_signal_fd = -1;
	}
	
	if (_epfd >= 0) {
		close(_epfd);
		_epfd = -1;
	}
	
	_started = false;
	_running = false;
	_stopped = true;
	
	trigger_event(SKLL_ON_SHUTDOWN);
	
	return 0;
}

void SkllNetwork::setup_signals() {
	_signal_fd = SkllSignals::setup();
	if (_signal_fd >= 0) {
		add_to_epoll(_signal_fd, EPOLLIN);
	}
}

SkllNetwork &SkllNetwork::on(int event, SkllHook::Callback callback, void *user_data) {
	_hook.on(event, callback, user_data);
	return *this;
}

void SkllNetwork::trigger_event(int type, SkllProtocol *proto, int error) {
	SkllEvent event;
	event.type = type;
	event.network = this;
	event.server = _server;
	event.protocol = proto;
	event.error_code = error;
	
	_hook.trigger(type, &event);
	if (proto) proto->_hook.trigger(type, &event);
}

SkllProtocol &SkllNetwork::listen(SkllProtocol &protocol) {
	protocol.set_network(this);
	_protocols[protocol.get_name()] = &protocol;
	return (protocol);
}

void SkllNetwork::remove_protocol(SkllProtocol *protocol) {
	if (!protocol) return;
	
	_protocols.erase(protocol->get_name());
	_fd_to_protocol.erase(protocol->get_fd());
	remove_from_epoll(protocol->get_fd());
}

SkllProtocol *SkllNetwork::get_protocol(const std::string &name) {
	std::map<std::string, SkllProtocol*>::iterator it = _protocols.find(name);
	return (it != _protocols.end()) ? it->second : NULL;
}

SkllProtocol *SkllNetwork::get_protocol(int fd) {
	std::map<int, SkllProtocol*>::iterator it = _fd_to_protocol.find(fd);
	return (it != _fd_to_protocol.end()) ? it->second : NULL;
}

void SkllNetwork::add_to_epoll(int fd, uint32_t events) {
	epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	
	epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
}

void SkllNetwork::remove_from_epoll(int fd) {
	epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
}

void SkllNetwork::update() {
	if (!_running) return;
	
	trigger_event(SKLL_ON_UPDATE);
	
	int n = epoll_wait(_epfd, &_ev[0], _queue, _timeout);
	
	if (n < 0) {
		if (errno == EINTR) return;
		trigger_event(SKLL_ON_ERROR, NULL, errno);
		return;
	}
	
	if (n == 0) {
		trigger_event(SKLL_ON_TIMEOUT);
		return;
	}
	
	for (int i = 0; i < n; ++i) {
		handle_event(_ev[i].data.fd, _ev[i].events);
	}
}

void SkllNetwork::handle_event(int fd, uint32_t events) {

	if (events  &EPOLLERR) {
		if (_fd_to_protocol.find(fd) == _fd_to_protocol.end()) {
			for (std::map<std::string, SkllProtocol*>::iterator pit = _protocols.begin();
				 pit != _protocols.end(); ++pit) {
				
				SkllClient *client = pit->second->get_client(fd);
				if (client) {
					SkllEvent event;
					event.type = SKLL_ON_DISCONNECT;
					event.network = this;
					event.server = _server;
					event.protocol = pit->second;
					event.client = client;
					event.fd = fd;
					
					_hook.trigger(SKLL_ON_DISCONNECT, &event);
					pit->second->_hook.trigger(SKLL_ON_DISCONNECT, &event);
					
					pit->second->remove_client(fd);
					delete client;
					break;
				}
			}
			
			remove_from_epoll(fd);
			close(fd);
		}
		return;
	}

	if (events  &(EPOLLHUP | EPOLLRDHUP)) {
		if (_fd_to_protocol.find(fd) == _fd_to_protocol.end()) {
			for (std::map<std::string, SkllProtocol*>::iterator pit = _protocols.begin();
				 pit != _protocols.end(); ++pit) {
				
				SkllClient *client = pit->second->get_client(fd);
				if (client) {
					SkllEvent event;
					event.type = SKLL_ON_DISCONNECT;
					event.network = this;
					event.server = _server;
					event.protocol = pit->second;
					event.client = client;
					event.fd = fd;
					
					_hook.trigger(SKLL_ON_DISCONNECT, &event);
					pit->second->_hook.trigger(SKLL_ON_DISCONNECT, &event);
					
					pit->second->remove_client(fd);
					delete client;
					break;
				}
			}
			
			remove_from_epoll(fd);
			close(fd);
		}
		return;
	}
	if (fd == _signal_fd) {
		handle_signal();
		return;
	}
	if (events  &EPOLLIN) {
		if (_fd_to_protocol.find(fd) != _fd_to_protocol.end()) {
			handle_accept(fd);
		} else {
			handle_client_data(fd);
		}
	}
}

void SkllNetwork::handle_accept(int listen_fd) {
	SkllProtocol *proto = _fd_to_protocol[listen_fd];
	if (!proto) return;
	
	while (true) {
		sockaddr_storage addr;
		socklen_t len = sizeof(addr);
		
		int client_fd = accept(listen_fd, (sockaddr*)&addr, &len);
		if (client_fd < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) break;
			return;
		}
		
		fcntl(client_fd, F_SETFL, O_NONBLOCK);
		
		char ip[INET6_ADDRSTRLEN];
		int port;
		
		if (addr.ss_family == AF_INET) {
			sockaddr_in *s = (sockaddr_in*)&addr;
			inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
			port = ntohs(s->sin_port);
		} else {
			sockaddr_in6 *s = (sockaddr_in6*)&addr;
			inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));
			port = ntohs(s->sin6_port);
		}
		
		std::ostringstream oss;
		oss << ip << ":" << port;
		std::string client_id = oss.str();
		
		SkllClient *client = new SkllClient(client_fd, client_id);
		client->recv_msg.set_protocol(proto);
		client->send_msg.set_protocol(proto);
		
		proto->add_client(client_fd, client);
		add_to_epoll(client_fd, _events);
		
		SkllEvent event;
		event.type = SKLL_ON_CONNECT;
		event.network = this;
		event.server = _server;
		event.protocol = proto;
		event.client = client;
		event.fd = client_fd;
		
		_hook.trigger(SKLL_ON_CONNECT, &event);
		proto->_hook.trigger(SKLL_ON_CONNECT, &event);
	}
}

void SkllNetwork::handle_client_data(int client_fd) {
	SkllProtocol *proto = NULL;
	SkllClient *client = NULL;
	
	for (std::map<std::string, SkllProtocol*>::iterator it = _protocols.begin();
		 it != _protocols.end(); ++it) {
		
		client = it->second->get_client(client_fd);
		if (client) {
			proto = it->second;
			break;
		}
	}
	
	if (!client || !proto) return;
	
	uint8_t buffer[4096];
	
	while (true) {
		ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
		
		if (n > 0) {
			proto->on_recv_data(client_fd, buffer, n);
			
		} else if (n == 0) {
			SkllEvent event;
			event.type = SKLL_ON_DISCONNECT;
			event.network = this;
			event.server = _server;
			event.protocol = proto;
			event.client = client;
			event.fd = client_fd;
			
			_hook.trigger(SKLL_ON_DISCONNECT, &event);
			proto->_hook.trigger(SKLL_ON_DISCONNECT, &event);
			
			proto->remove_client(client_fd);
			remove_from_epoll(client_fd);
			close(client_fd);
			delete client;
			return;
			
		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			SkllEvent event;
			event.type = SKLL_ON_DISCONNECT;
			event.network = this;
			event.server = _server;
			event.protocol = proto;
			event.client = client;
			event.fd = client_fd;
			event.error_code = errno;
			
			_hook.trigger(SKLL_ON_DISCONNECT, &event);
			proto->_hook.trigger(SKLL_ON_DISCONNECT, &event);
			
			proto->remove_client(client_fd);
			remove_from_epoll(client_fd);
			close(client_fd);
			delete client;
			return;
		}
	}
}

void SkllNetwork::handle_signal() {
	int sig = SkllSignals::read_signal();
	if (sig < 0) return;
	
	SkllEvent event;
	event.type = SKLL_ON_SIGNAL;
	event.network = this;
	event.server = _server;
	event.signal_num = sig;
	
	_hook.trigger(SKLL_ON_SIGNAL, &event);
	
	if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
		if (_server) {
			_server->stop();
		} else {
			_running = false;
		}
	}
}

void SkllNetwork::set_server(SkllServer *server) {
	_server = server;
}

SkllServer *SkllNetwork::get_server() const {
	return _server;
}

int SkllNetwork::get_epoll_fd() const { return _epfd; }
const std::string &SkllNetwork::get_name() const { return _name; }
bool SkllNetwork::is_started() const { return _started; }
bool SkllNetwork::is_running() const { return _running; }