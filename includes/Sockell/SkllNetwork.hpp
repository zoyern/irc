/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllNetwork.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 16:27:41 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 16:27:41 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllRouter.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllClient.hpp>
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <ctime>
#include <sys/epoll.h>

class SkllServer; /* Forward declaration */

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   DEFAULTS (can be overridden via set_* methods)                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

#define SKLL_DEFAULT_MAX_EVENTS		1024
#define SKLL_DEFAULT_CONN_WINDOW	60
#define SKLL_DEFAULT_MSG_WINDOW		1

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   RATE LIMIT - Protection against DDoS and flooding                         */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct SkllRateLimit {
	size_t			count;          /* Number of events in window       */
	time_t			window_start;   /* Start of current window          */
	
	SkllRateLimit() : count(0), window_start(0) {}
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   NETWORK - epoll wrapper with protocols and clients                        */
/*   • Each network has its own epoll                                          */
/*   • Can handle multiple protocols (TCP, UDP) on same network                */
/*   • Clients stored in map (fd -> client), NO FIXED LIMIT                    */
/*   • Built-in rate limiting per IP and per client                            */
/*   • Dynamic event buffer (no fixed array)                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllNetwork {
public:
	SkllNetwork();
	SkllNetwork(const std::string &name);
	~SkllNetwork();

	/* Configuration */
	SkllNetwork &				set_name(const std::string &name);
	SkllNetwork &				set_timeout(int ms);
	SkllNetwork &				set_delim(const std::string &d);
	SkllNetwork &				set_max_events(size_t n);
	void						set_server(SkllServer *s);
	void						disconnect(SkllEvent &e);

	/* Getters */
	const std::string &			name() const;
	int							timeout() const;
	const std::string &			delim() const;
	size_t						max_events() const;
	fd_t						epoll_fd() const;
	SkllServer *				server();

	/* Protocol management */
	SkllNetwork &				add(SkllProtocol &proto);
	size_t						protocol_count() const;

	/* Client management - O(log n) operations */
	SkllClient &				add_client(fd_t fd);
	SkllClient *				get_client(fd_t fd);
	void						remove_client(fd_t fd);
	size_t						client_count() const;
	
	/* Typed client data access - NO CAST NEEDED */
	template<typename T>
	T *client_data(fd_t fd) {
		SkllClient *c = get_client(fd);
		return c ? static_cast<T*>(c->raw_data()) : 0;
	}

	/* Rate limiting */
	SkllNetwork &				set_conn_limit(size_t max_per_ip, int window_sec);
	SkllNetwork &				set_msg_limit(size_t max_per_client, int window_sec);
	bool						check_conn_rate(const std::string &ip);
	bool						check_msg_rate(fd_t fd);
	void						clear_rate_limits();

	/* Iteration */
	std::map<fd_t, SkllClient> &		clients();
	const std::map<fd_t, SkllClient> &	clients() const;

	/* Event hooks */
	SkllNetwork &				on(int type, SkllCallback fn);
	
	/* Typed callback with user data - NO CAST NEEDED */
	template<typename T>
	SkllNetwork &				on(int type, int (*fn)(SkllEvent &, T *), T *data) {
		hooks(type).add(fn, data);
		return *this;
	}
	
	SkllHooks &					hooks(int type);

	/* Router */
	SkllRouter &				router();

	/* Lifecycle */
	bool						init();
	void						close();

	/* Event loop step (called by server) */
	int							poll_once();

private:
	SkllServer *				_server;
	std::string					_name;
	int							_timeout;
	std::string					_delim;
	fd_t						_epoll_fd;
	size_t						_max_events;

	/* Dynamic event buffer - NO FIXED LIMIT */
	std::vector<struct epoll_event>		_events;

	std::deque<SkllProtocol *>			_protocols;
	std::map<fd_t, SkllClient>			_clients;
	std::map<fd_t, SkllProtocol *>		_listen_fds;

	SkllRouter					_router;
	SkllHooks					_on_accept;
	SkllHooks					_on_close;
	SkllHooks					_on_message;
	SkllHooks					_on_error;

	/* Rate limiting */
	std::map<std::string, SkllRateLimit>	_conn_rates;  /* IP -> rate    */
	std::map<fd_t, SkllRateLimit>			_msg_rates;   /* fd -> rate    */
	size_t									_conn_limit;  /* Max conn/IP   */
	int										_conn_window; /* Window (sec)  */
	size_t									_msg_limit;   /* Max msg/client*/
	int										_msg_window;  /* Window (sec)  */

	void						_handle_accept(SkllProtocol &proto);
	void						_handle_read(fd_t fd);
	void						_handle_udp_read(SkllProtocol &proto);
	void						_handle_close(fd_t fd);
	
	/* Track which protocol each fd belongs to */
	std::map<fd_t, SkllProtocol *>		_fd_to_proto;

	SkllNetwork(const SkllNetwork &);
	SkllNetwork &operator=(const SkllNetwork &);
};
