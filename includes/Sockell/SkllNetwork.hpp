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
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllSignals.hpp>
#include <sys/epoll.h>
#include <string>
#include <vector>
#include <map>

class SkllServer;

class SkllNetwork {
private:
	int _epfd;
	int _signal_fd;
	std::string _name;
	int _timeout;
	int _queue;
	uint32_t _events;
	
	bool _started;
	bool _running;
	bool _stopped;
	
	SkllServer *_server;
	
	std::vector<epoll_event> _ev;
	std::map<std::string, SkllProtocol*> _protocols;
	std::map<int, SkllProtocol*> _fd_to_protocol;
	
	SkllHook _hook;

public:
	SkllNetwork();
	SkllNetwork(const std::string &name, int timeout = 100, int queue = 128);
	~SkllNetwork();
	
	int start();
	int run();
	int stop();
	int shutdown();
	
	SkllNetwork &on(int event, SkllHook::Callback callback, void *user_data = NULL);
	
	void set_server(SkllServer *server);
	SkllServer *get_server() const;
	
	SkllProtocol &listen(SkllProtocol &protocol);
	void remove_protocol(SkllProtocol *protocol);
	SkllProtocol *get_protocol(const std::string &name);
	SkllProtocol *get_protocol(int fd);
	
	const std::map<std::string, SkllProtocol*> &get_protocols() const;
	
	void add_to_epoll(int fd, uint32_t events);
	void remove_from_epoll(int fd);
	
	void update();
	
	int get_epoll_fd() const;
	const std::string &get_name() const;
	bool is_started() const;
	bool is_running() const;

private:
	void setup_signals();
	void handle_event(int fd, uint32_t events);
	void handle_accept(int listen_fd);
	void handle_client_data(int client_fd);
	void handle_signal();
	void trigger_event(int type, SkllProtocol *proto = NULL, int error = 0);
	void print_startup_info();
	
	SkllNetwork(const SkllNetwork&);
	SkllNetwork &operator=(const SkllNetwork&);
};