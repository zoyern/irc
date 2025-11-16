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

SkllNetwork::SkllNetwork()
    : _name(""), _epfd(-1), _timeout(100), _queue(128)
    , _epoll_events(EPOLLIN | EPOLLRDHUP | EPOLLET), _server(NULL)
{
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) throw SkllErrorEpoll(_get_epoll_error());
    _events.resize(_queue);
}

SkllNetwork::SkllNetwork(const std::string& name, int timeout, int queue)
    : _name(name), _epfd(-1), _timeout(timeout), _queue(queue)
    , _epoll_events(EPOLLIN | EPOLLRDHUP | EPOLLET), _server(NULL)
{
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) throw SkllErrorEpoll(_get_epoll_error());
    _events.resize(_queue);
}

SkllNetwork::~SkllNetwork() {
    if (_epfd >= 0) close(_epfd);
}

SkllNetwork::SkllNetwork(const SkllNetwork& other)
    : _name(other._name)
    , _epfd(-1)
    , _timeout(other._timeout)
    , _queue(other._queue)
    , _epoll_events(other._epoll_events)
    , _server(NULL)
{
    // Recrée epoll
    _epfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epfd < 0) throw SkllErrorEpoll(_get_epoll_error());
    _events.resize(_queue);
}

SkllNetwork& SkllNetwork::operator=(const SkllNetwork& other) {
    if (this != &other) {
        _name = other._name;
        _timeout = other._timeout;
        _queue = other._queue;
        _epoll_events = other._epoll_events;
        _events.resize(_queue);
    }
    return *this;
}

void SkllNetwork::set_server(SkllServer* srv) {
    _server = srv;
}

SkllProtocol& SkllNetwork::listen(const std::string& name, const std::string& addr, int port, int opts) {
    _protocols_owned[name] = SkllProtocol();
    _protocols_owned[name].create(name, addr, port, opts);
    return _protocols_owned[name];
}

SkllProtocol& SkllNetwork::listen(SkllProtocol* proto) {
    if (!proto) throw SkllException("NULL protocol");
    _protocols_owned[proto->get_name()] = *proto;
    return _protocols_owned[proto->get_name()];
}

SkllNetwork& SkllNetwork::on(SkllEvent event, SkllCallback callback, void* data) {
    hook.on(event, callback, data);
    return *this;
}

void SkllNetwork::run() {
    for (std::map<std::string, SkllProtocol>::iterator it = _protocols_owned.begin();
         it != _protocols_owned.end(); ++it) {
        it->second.run();
        _fd_to_protocol[it->second.get_fd()] = &it->second;
        add_event(it->second.get_fd());
    }
    
    hook.trigger(ON_START);
}

void SkllNetwork::update() {
    hook.trigger(ON_UPDATE);
    
    int n = epoll_wait(_epfd, &_events[0], _queue, _timeout);
    if (n < 0) {
        if (errno == EINTR) return;
        hook.trigger(ON_ERROR);
        return;
    }
    if (n == 0) {
        hook.trigger(ON_TIMEOUT);
        return;
    }
    
    for (int i = 0; i < n; i++) {
        int fd = _events[i].data.fd;
        uint32_t events = _events[i].events;
        
        if (events & EPOLLERR) {
            hook.trigger(ON_ERROR);
            destroy_event(fd);
            close(fd);
            continue;
        }
        
        if (events & (EPOLLHUP | EPOLLRDHUP)) {
            hook.trigger(ON_DISCONNECT);
            destroy_event(fd);
            close(fd);
            continue;
        }
        
        if (_fd_to_protocol.find(fd) != _fd_to_protocol.end()) {
            _handle_listening(fd, events);
        } else {
            _handle_client(fd, events);
        }
    }
}

void SkllNetwork::_handle_listening(int fd, uint32_t events) {
    if (events & EPOLLIN) {
        _accept_tcp(fd);
    }
}

void SkllNetwork::_handle_client(int fd, uint32_t events) {
    if (events & EPOLLIN) {
        _recv_tcp(fd);
    }
}

void SkllNetwork::_accept_tcp(int listen_fd) {
    while (true) {
        struct sockaddr_storage addr;
        socklen_t len = sizeof(addr);
        
        int client_fd = accept(listen_fd, (struct sockaddr*)&addr, &len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            return;
        }
        
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        add_event(client_fd);
        
        hook.trigger(ON_CONNECT);
        std::cout << "[+] Client connected: fd=" << client_fd << std::endl;
    }
}

void SkllNetwork::_recv_tcp(int fd) {
    char buffer[4096];
    std::string accumulated;
    
    while (true) {
        ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        
        if (n > 0) {
            buffer[n] = '\0';
            accumulated.append(buffer, n);
        } else if (n == 0) {
            hook.trigger(ON_DISCONNECT);
            destroy_event(fd);
            close(fd);
            return;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            hook.trigger(ON_ERROR);
            destroy_event(fd);
            close(fd);
            return;
        }
    }
    
    if (!accumulated.empty()) {
        hook.trigger(ON_RECV);
        std::cout << "[>] fd=" << fd << ": " << accumulated;
    }
}

SkllNetwork& SkllNetwork::set_timeout(int ms) {
    _timeout = ms;
    return *this;
}

SkllNetwork& SkllNetwork::set_queue(int size) {
    _queue = size;
    _events.resize(size);
    return *this;
}

SkllNetwork& SkllNetwork::set_events(uint32_t events) {
    _epoll_events = events;
    return *this;
}

SkllNetwork& SkllNetwork::set_edge_triggered(bool enable) {
    if (enable) _epoll_events |= EPOLLET;
    else _epoll_events &= ~EPOLLET;
    return *this;
}

SkllNetwork& SkllNetwork::set_oneshot(bool enable) {
    if (enable) _epoll_events |= EPOLLONESHOT;
    else _epoll_events &= ~EPOLLONESHOT;
    return *this;
}

SkllNetwork& SkllNetwork::set_exclusive(bool enable) {
    if (enable) _epoll_events |= EPOLLEXCLUSIVE;
    else _epoll_events &= ~EPOLLEXCLUSIVE;
    return *this;
}

std::string SkllNetwork::get_name() const { return _name; }
int SkllNetwork::get_epfd() const { return _epfd; }
int SkllNetwork::get_timeout() const { return _timeout; }
int SkllNetwork::get_queue() const { return _queue; }
uint32_t SkllNetwork::get_events() const { return _epoll_events; }
int SkllNetwork::count_protocols() const { return _protocols_owned.size(); }

void SkllNetwork::add_event(int fd) {
    epoll_event ev;
    ev.events = _epoll_events;
    ev.data.fd = fd;
    epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
}

void SkllNetwork::destroy_event(int fd) {
    epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
}

std::string SkllNetwork::_get_epoll_error() {
    std::ostringstream oss;
    oss << "epoll_create1() failed: ";
    switch (errno) {
        case EINVAL: oss << "Invalid flags"; break;
        case EMFILE: oss << "Process epoll limit reached"; break;
        case ENFILE: oss << "System FD limit reached"; break;
        case ENOMEM: oss << "Insufficient memory"; break;
        default: oss << std::strerror(errno);
    }
    return oss.str();
}