/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 18:52:14 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 18:52:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllChannel.hpp>
#include <Sockell/SkllClient.hpp>

SkllChannel::SkllChannel() : name(""), userdata(NULL) {}

SkllChannel::SkllChannel(const std::string &n) : name(n), userdata(NULL) {}

SkllChannel::~SkllChannel() {}

SkllChannel::SkllChannel(const SkllChannel&) {}
SkllChannel	&SkllChannel::operator=(const SkllChannel&) { return (*this); }

SkllChannel &SkllChannel::on(int event, SkllHook::Callback cb, void *user_data) {
    hook.on(event, cb, user_data);
    return *this;
}

void SkllChannel::add_client(int fd, SkllClient *client) {
    clients[fd] = client;
}

void SkllChannel::remove_client(int fd) {
    clients.erase(fd);
}

SkllClient *SkllChannel::get_client(int fd) {
    std::map<int, SkllClient*>::iterator it = clients.find(fd);
    return (it != clients.end()) ? it->second : NULL;
}

bool SkllChannel::has_client(int fd) const {
    return clients.find(fd) != clients.end();
}

size_t SkllChannel::client_count() const {
    return clients.size();
}

void SkllChannel::broadcast(const std::string &msg, int exclude_fd) {
    for (std::map<int, SkllClient*>::iterator it = clients.begin();
         it != clients.end(); ++it) {
        if (it->first != exclude_fd) {
            it->second->send(msg);
        }
    }
}