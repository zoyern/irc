/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 03:55:49 by marvin            #+#    #+#             */
/*   Updated: 2025/11/06 03:55:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllProtocol.hpp>

SkllRateLimit::SkllRateLimit()
	: max_messages(0), window_seconds(0) {}

SkllRateLimit::SkllRateLimit(size_t max_msg, time_t window)
	: max_messages(max_msg), window_seconds(window) {}
	
bool SkllRateLimit::is_allowed() {
	if (max_messages == 0) return true;
	
	time_t now = std::time(NULL);
	time_t threshold = now - window_seconds;

	while (!timestamps.empty() && timestamps.front() < threshold) {
		timestamps.pop_front();
	}
	
	return timestamps.size() < max_messages;
}

void SkllRateLimit::record() {
	if (max_messages == 0) return;
	timestamps.push_back(std::time(NULL));
}

void SkllRateLimit::reset() {
	timestamps.clear();
}

SkllClient::SkllClient()
	: fd(-1), id(""), userdata(NULL) {}

SkllClient::SkllClient(int client_fd, const std::string &client_id)
	: fd(client_fd), id(client_id), userdata(NULL) {
	recv_msg.set_fd(client_fd);
	send_msg.set_fd(client_fd);
}

SkllClient::~SkllClient() {}

SkllClient::SkllClient(const SkllClient&) {}
SkllClient	&SkllClient::operator=(const SkllClient&) { return (*this); }

SkllClient &SkllClient::on(int event, SkllHook::Callback cb, void *user_data) {
	hook.on(event, cb, user_data);
	return (*this);
}

void SkllClient::send(const std::string &data) {
	send_msg.clear();
	send_msg << data;
	send_msg.send();
}

void SkllClient::send(SkllMessage &msg) {
	msg.set_fd(fd);
	msg.send();
}

void SkllClient::set_rate_limit(size_t max_messages, time_t window_seconds) {
	rate_limit = SkllRateLimit(max_messages, window_seconds);
}

bool SkllClient::is_rate_limited() {
	return !rate_limit.is_allowed();
}

int SkllClient::get_fd() const { return fd; }
const std::string &SkllClient::get_id() const { return id; }

void SkllClient::set_fd(int client_fd) {
	fd = client_fd;
	recv_msg.set_fd(client_fd);
	send_msg.set_fd(client_fd);
}

void SkllClient::set_id(const std::string &client_id) {
	id = client_id;
}