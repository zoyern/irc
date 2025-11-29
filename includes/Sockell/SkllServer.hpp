/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:09:24 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:09:24 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllChannels.hpp>
#include <Sockell/SkllSignals.hpp>
#include <deque>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SERVER - Main server managing networks, channels, and signals             */
/*   • FD limit via rlimit                                                     */
/*   • Multiple networks (each with own epoll)                                 */
/*   • Global channel management                                               */
/*   • Signal handling                                                         */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllServer {
public:
	SkllServer();
	~SkllServer();

	/* Configuration */
	SkllServer &			set_fd_limit(size_t limit);

	/* Network management */
	SkllServer &			add(SkllNetwork &net);
	size_t					network_count() const;

	/* Channel management (global) */
	SkllChannels &			channels();

	/* Default channel for new connections */
	SkllServer &			set_default_channel(const std::string &name);
	const std::string &		default_channel() const;

	/* Signals */
	SkllSignals &			signals();

	/* Event hooks */
	SkllServer &			on(int type, SkllCallback fn);

	/* Lifecycle */
	bool					init();
	int						run();
	void					stop();
	bool					running() const;

	/* User data (template, no void* cast) */
	template <typename T>
	void					set_data(T *ptr);

	template <typename T>
	T *						data();

private:
	std::deque<SkllNetwork *>	_networks;
	SkllChannels				_channels;
	std::string					_default_channel;
	SkllSignals					_signals;
	SkllHooks					_on_start;
	SkllHooks					_on_stop;
	SkllHooks					_on_signal;
	bool						_running;
	void *						_data;

	void						_handle_signal();

	SkllServer(const SkllServer &);
	SkllServer &operator=(const SkllServer &);
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   TEMPLATE IMPLEMENTATIONS                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

template <typename T>
void SkllServer::set_data(T *ptr) {
	_data = ptr;
}

template <typename T>
T *SkllServer::data() {
	return static_cast<T *>(_data);
}
