/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 00:46:03 by marvin            #+#    #+#             */
/*   Updated: 2025/11/08 00:46:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

Epoll::Epoll() {
	_epfd = epoll_create(1);
	if (_epfd < 0)
		throw std::runtime_error("epoll_create() failed");
}

Epoll::~Epoll() {
	if (_epfd >= 0)
		close(_epfd);
}

void Epoll::add(int fd, uint32_t events) {
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw std::runtime_error("epoll_ctl(ADD) failed");
}

void Epoll::del(int fd) {
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("epoll_ctl(DEL) failed");
}

int Epoll::wait(int timeout) {
	return epoll_wait(_epfd, _events, MAX_EVENTS, timeout);
}

struct epoll_event* Epoll::events() { return _events; }
int Epoll::fd() const { return _epfd; }