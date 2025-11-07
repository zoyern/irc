/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:17:56 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:17:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <irc.hpp>

enum Protocol {
    PROTO_TCP,
    PROTO_UDP,
    PROTO_BOTH = PROTO_TCP | PROTO_UDP
};

#ifndef SETTINGS
	#define NAME			"irc"
	#define ADDRESS			"0.0.0.0"
	#define MSG				"Welcome to the IRC server!"
	#define MAX_CLIENTS		-1
	#define RESERVED_FD		10
	#define TIMEOUT			300
	#define QUEUE 			128
#endif


enum e_event {
	ON_START   = 1 << 0,  // 0001 = 1
    ON_UPDATE    = 1 << 1,  // 0010 = 2 
    ON_SHUTDOWN = 1 << 2,  // 0100 = 4
    ON_CONNECT   = 1 << 3   // 1000 = 8
    ON_DISCONNECT   = 1 << 4   // 10000 = 16
};

class Server {
	public:
		typedef	void	(*Callback)(Server &server, void* data);
	private:
		int 		_fd;
		int 		_epfd;
		bool 		_running;

		uint16_t	_port;
		std::string	_password;
		std::string	_name;
		std::string	_address;
		std::string	_connexion_msg;
		int			_max_clients;
		int			_reserved_fds;
		int 		_timeout;
		int 		_queue;

		Console							*_console;
		Channel							*_default_channel;
		std::map<std::string, Channel*>	_channels;
		std::map<int, Client*>			_clients;

	public:
	    ~Server();
		Server(const std::string &port, const std::string &password);
		Server(const Server& other);
		Server&	operator=(const Server& other);

		Server&	port(const std::string& port);
		Server&	password(const std::string& pass);
		Server&	name(const std::string& name);
		Server& address(const std::string& address);
		Server& connexion_msg(const std::string &msg);
		Server& max_clients(int max);
		Server& reserved_fds(int reserved);
		Server& timeout(int seconds);
		Server& queue(int backlog);
		Server& hook(e_event event, Callback callback, void* data = NULL);

		Channel& channel(const std::string &name, bool is_default = false);

		int		run();
		void	stop();
	private:
		uint16_t 	check_port(const std::string &port);
		void		_update_reserved_fds(int delta);

		void _init_socket();
		void _init_epoll();
		void _init_limits();
		void _init_console();

};