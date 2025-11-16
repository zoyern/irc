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

SkllChannel::SkllChannel() : name(""), topic(""), password("") {}
SkllChannel::SkllChannel(const std::string& n) : name(n), topic(""), password("") {}
SkllChannel::~SkllChannel() {}

// Constructeur copie
SkllChannel::SkllChannel(const SkllChannel& other)
    : name(other.name), topic(other.topic), password(other.password)
{}

SkllChannel& SkllChannel::operator=(const SkllChannel& other) {
    if (this != &other) {
        name = other.name;
        topic = other.topic;
        password = other.password;
    }
    return *this;
}

SkllChannel& SkllChannel::set_topic(const std::string& t) {
    topic = t;
    return *this;
}

SkllChannel& SkllChannel::set_password(const std::string& p) {
    password = p;
    return *this;
}

SkllChannel& SkllChannel::op(int flags) {
    (void)flags;
    return *this;
}

SkllChannel& SkllChannel::size(int max_users) {
    (void)max_users;
    return *this;
}

SkllChannel& SkllChannel::invite_only(bool enable) {
    (void)enable;
    return *this;
}

SkllChannel& SkllChannel::restricted_topic(bool enable) {
    (void)enable;
    return *this;
}