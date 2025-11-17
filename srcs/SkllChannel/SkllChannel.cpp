/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllChannel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 20:47:38 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 20:47:38 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllChannel.hpp>

SkllChannel::SkllChannel() : name(""), userdata(NULL) {}
SkllChannel::SkllChannel(const std::string& n) : name(n), userdata(NULL) {}
SkllChannel::~SkllChannel() {}

void SkllChannel::add_client(const std::string& id) {
    client_ids.insert(id);
}

void SkllChannel::remove_client(const std::string& id) {
    client_ids.erase(id);
}

bool SkllChannel::has_client(const std::string& id) const {
    return client_ids.find(id) != client_ids.end();
}

size_t SkllChannel::size() const {
    return client_ids.size();
}

SkllChannel& SkllChannel::on(int event, SkllHook::Callback cb, void* user_data) {
    hook.on(event, cb, user_data);
    return *this;
}