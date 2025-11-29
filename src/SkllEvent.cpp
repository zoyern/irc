/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllEvent.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:51:10 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:51:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllChannel.hpp>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONSTRUCTOR / DESTRUCTOR                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllEvent::SkllEvent()
	: _event_type(SKLL_EV_NONE), _fd(-1), _signal(0), _udp(false)
	, _line_ending("\r\n"), _server(0), _network(0), _client(0) {}

SkllEvent::~SkllEvent() {}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONNECTION INFO - Read-only access to client connection                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

fd_t SkllEvent::fd() const { return _fd; }

const std::string &SkllEvent::ip() const {
	static std::string empty;
	return _client ? _client->ip() : empty;
}

int SkllEvent::event_type() const { return _event_type; }
int SkllEvent::signal_number() const { return _signal; }
bool SkllEvent::is_udp() const { return _udp; }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   MESSAGE ACCESS - Parsed IRC message                                       */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllMessage &SkllEvent::message() { return _message; }
const SkllMessage &SkllEvent::message() const { return _message; }

SkllSpan SkllEvent::command() const { return _message.cmd(); }
size_t SkllEvent::arg_count() const { return _message.argc(); }
SkllSpan SkllEvent::argument(size_t index) const { return _message.arg(index); }
SkllSpan SkllEvent::trailing() const { return _message.trailing(); }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SEND API - Fluent message sending                                         */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllSender SkllEvent::send() {
	return SkllSender(_fd, _line_ending);
}

SkllSender SkllEvent::send(fd_t target_fd) {
	return SkllSender(target_fd, _line_ending);
}

void SkllEvent::set_line_ending(const std::string &ending) {
	_line_ending = ending;
}

const std::string &SkllEvent::line_ending() const {
	return _line_ending;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CHANNEL ACCESS - Channel management                                       */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllChannels &SkllEvent::channels() {
	return _server->channels();
}

SkllChannel *SkllEvent::channel(const std::string &name) {
	return _server ? _server->channels().get(name) : 0;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SERVER/NETWORK ACCESS - Library components                                */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllServer *SkllEvent::server() { return _server; }
SkllNetwork *SkllEvent::network() { return _network; }
const SkllServer *SkllEvent::server() const { return _server; }
const SkllNetwork *SkllEvent::network() const { return _network; }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   INTERNAL - Library use only                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

void SkllEvent::set_event_type(int type) { _event_type = type; }
void SkllEvent::set_fd(fd_t file_descriptor) { _fd = file_descriptor; }
void SkllEvent::set_server(SkllServer *srv) { _server = srv; }
void SkllEvent::set_network(SkllNetwork *net) { _network = net; }
void SkllEvent::set_client(SkllClient *cli) { _client = cli; }
void SkllEvent::set_signal_number(int sig) { _signal = sig; }
void SkllEvent::set_udp(bool udp) { _udp = udp; }

SkllClient *SkllEvent::internal_client() { return _client; }
const SkllClient *SkllEvent::internal_client() const { return _client; }

void SkllEvent::reset() {
	_event_type = SKLL_EV_NONE;
	_fd = -1;
	_signal = 0;
	_udp = false;
	_server = 0;
	_network = 0;
	_client = 0;
	_message.reset();
}
