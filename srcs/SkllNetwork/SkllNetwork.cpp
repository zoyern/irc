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
#include <Sockell/SkllMessage.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <iostream>

SkllNetwork::SkllNetwork()
    : _epfd(-1), _timeout(100), _queue(128), _events(EPOLLIN | EPOLLRDHUP | EPOLLET), _server(NULL)
{
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) throw SkllErrorEpoll(_err_epoll());
    _ev.resize(_queue);
}

SkllNetwork::SkllNetwork(const std::string& n, int t, int q)
    : _name(n), _epfd(-1), _timeout(t), _queue(q), _events(EPOLLIN | EPOLLRDHUP | EPOLLET), _server(NULL)
{
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) throw SkllErrorEpoll(_err_epoll());
    _ev.resize(_queue);
}

SkllNetwork::SkllNetwork(const SkllNetwork& other)
    : _name(other._name), _epfd(-1), _timeout(other._timeout), _queue(other._queue),
      _events(other._events), _server(other._server), _protos(other._protos), _fd_proto(other._fd_proto)
{
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) throw SkllErrorEpoll(_err_epoll());
    _ev.resize(_queue);
}

SkllNetwork& SkllNetwork::operator=(const SkllNetwork& other) {
    if (this != &other) {
        if (_epfd >= 0) close(_epfd);
        _name = other._name;
        _timeout = other._timeout;
        _queue = other._queue;
        _events = other._events;
        _server = other._server;
        _protos = other._protos;
        _fd_proto = other._fd_proto;
        
        _epfd = epoll_create1(EPOLL_CLOEXEC);
        if (_epfd < 0) throw SkllErrorEpoll(_err_epoll());
        _ev.resize(_queue);
    }
    return *this;
}

SkllNetwork::~SkllNetwork() {
    if (_epfd >= 0) close(_epfd);
}

void SkllNetwork::set_server(SkllServer* s) {
    _server = s;
}

SkllProtocol	&SkllNetwork::listen(const SkllProtocol &protocol) {
	typedef std::map<std::string, SkllProtocol*>::iterator It;
    std::pair<It, bool> res = _protocols.insert(std::make_pair(protocol.get_name(), (SkllProtocol*)&protocol));
    _protocols_fd[protocol.get_fd()] = res.first->second;
    return (*(res.first->second));
}

SkllProtocol* SkllNetwork::get_protocol(const std::string& name) {
    std::map<std::string, SkllProtocol*>::iterator it = _protos.find(name);
    return (it != _protos.end()) ? it->second : NULL;
}

SkllNetwork& SkllNetwork::on(int event, SkllHook::Callback cb, void* user_data) {
    hook.on(event, cb, user_data);
    return *this;
}

void SkllNetwork::run() {
    for (std::map<std::string, SkllProtocol*>::iterator it = _protos.begin(); it != _protos.end(); ++it) {
        it->second->run();
        _fd_proto[it->second->get_fd()] = it->second;
        add_event(it->second->get_fd());
    }
    
    hook.trigger(ON_START, NULL);
}

void SkllNetwork::update() {
    hook.trigger(ON_UPDATE, NULL);
    
    int n = epoll_wait(_epfd, &_ev[0], _queue, _timeout);
    if (n < 0) {
        if (errno == EINTR) return;
        hook.trigger(ON_ERROR, NULL);
        return;
    }
    if (n == 0) {
        hook.trigger(ON_TIMEOUT, NULL);
        return;
    }
    
    for (int i = 0; i < n; ++i) {
        int fd = _ev[i].data.fd;
        uint32_t ev = _ev[i].events;
        
        if (ev & EPOLLERR) {
            hook.trigger(ON_ERROR, NULL);
            destroy_event(fd);
            close(fd);
            continue;
        }
        
        if (ev & (EPOLLHUP | EPOLLRDHUP)) {
            for (std::map<std::string, SkllProtocol*>::iterator pit = _protos.begin(); pit != _protos.end(); ++pit) {
                SkllClient* cli = pit->second->get_client(fd);
                if (cli) {
                    pit->second->hook.trigger(ON_DISCONNECT, NULL);
                    pit->second->remove_client(fd);
                    break;
                }
            }
            hook.trigger(ON_DISCONNECT, NULL);
            destroy_event(fd);
            close(fd);
            continue;
        }
        
        if (_fd_proto.find(fd) != _fd_proto.end()) _handle_listen(fd, ev);
        else _handle_client(fd, ev);
    }
}

void SkllNetwork::_handle_listen(int fd, uint32_t ev) {
    if (ev & EPOLLIN) _accept(fd);
}

void SkllNetwork::_handle_client(int fd, uint32_t ev) {
    if (ev & EPOLLIN) _recv(fd);
}

void SkllNetwork::_accept(int lfd) {
    SkllProtocol* proto = _fd_proto[lfd];
    
    while (true) {
        struct sockaddr_storage addr;
        socklen_t len = sizeof(addr);
        
        int cfd = accept(lfd, (struct sockaddr*)&addr, &len);
        if (cfd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            return;
        }
        
        fcntl(cfd, F_SETFL, O_NONBLOCK);
        add_event(cfd);

        hook.trigger(ON_CONNECT, &cfd);
        proto->hook.trigger(ON_CONNECT, &cfd);
    }
}

void SkllNetwork::_recv(int fd) {
    SkllProtocol* proto = NULL;
    SkllClient* cli = NULL;
    
    for (std::map<std::string, SkllProtocol*>::iterator it = _protos.begin(); it != _protos.end(); ++it) {
        cli = it->second->get_client(fd);
        if (cli) {
            proto = it->second;
            break;
        }
    }
    
    if (!cli) return;
    
    unsigned char buf[4096];
    
    while (true) {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        
        if (n > 0) {
            cli->buffer.insert(cli->buffer.end(), buf, buf + n);
        } else if (n == 0) {
            hook.trigger(ON_DISCONNECT, NULL);
            if (proto) {
                proto->hook.trigger(ON_DISCONNECT, NULL);
                proto->remove_client(fd);
            }
            destroy_event(fd);
            close(fd);
            return;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            hook.trigger(ON_ERROR, NULL);
            if (proto) proto->remove_client(fd);
            destroy_event(fd);
            close(fd);
            return;
        }
    }
    
    if (!cli->buffer.empty()) {
        hook.trigger(ON_RECV, NULL);
        if (proto) proto->hook.trigger(ON_RECV, NULL);
        cli->hook.trigger(ON_RECV, NULL);
    }
}

void SkllNetwork::broadcast(const char* d, size_t l) {
    for (std::map<std::string, SkllProtocol*>::iterator it = _protos.begin(); it != _protos.end(); ++it)
        it->second->broadcast(d, l);
}

void SkllNetwork::add_event(int fd) {
    epoll_event ev;
    ev.events = _events;
    ev.data.fd = fd;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
}

void SkllNetwork::destroy_event(int fd) {
    epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
}

std::string SkllNetwork::get_name() const { return _name; }
int SkllNetwork::get_epfd() const { return _epfd; }
size_t SkllNetwork::protocol_count() const { return _protos.size(); }

std::string SkllNetwork::print_protocols() const {
    std::ostringstream o;
    
    for (std::map<std::string, SkllProtocol*>::const_iterator it = _protos.begin(); it != _protos.end(); ++it) {
        SkllProtocol* p = it->second;
        
        o << "  [" << p->get_name() << "] ";
        o << p->get_address() << ":" << p->get_port() << " (";
        
        if (p->is_tcp()) o << "TCP";
        else o << "UDP";
        
        o << "/";
        
        if (p->get_address().find(':') != std::string::npos) o << "IPv6";
        else o << "IPv4";
        
        o << ")\n";
    }
    
    return o.str();
}

std::string SkllNetwork::_err_epoll() {
    std::ostringstream o;
    o << "epoll_create1(): ";
    switch (errno) {
        case EINVAL: o << "Invalid flags"; break;
        case EMFILE: o << "Process limit"; break;
        case ENFILE: o << "System limit"; break;
        default: o << std::strerror(errno);
    }
    return o.str();
}

int		SkllNetwork::start() { std::cout << "[Network] "<< _name << " Stating binding..." << std::endl;return(0);}
int		SkllNetwork::wait() {return(0);}
int		SkllNetwork::stop() {std::cout << "[Network] "<< _name << " stoped..." << std::endl;return(0);}
int		SkllNetwork::shutdown() {std::cout << "[Network] "<< _name << " Shutdown !" << std::endl;return(0);}

std::string	SkllNetwork::name() const {return (_name);}
int	SkllNetwork::fd() const{return (_epfd);}