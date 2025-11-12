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

#include <Sockell.hpp>
#include <Sockell/SkllErrors.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllConsole.hpp>

#define SKLL_NAME			"irc"
#define SKLL_ADDRESS		"0.0.0.0"
#define SKLL_MAX_CLIENTS	-1
#define SKLL_RESERVED_FD	10
#define SKLL_TIMEOUT		300
#define SKLL_QUEUE 			128

// ================================
// Sockell Network Class
// ================================
class SkllNetwork {
	// ================================
	// Public vars
	// ================================
	public:
		SkllErrors		errors;
		SkllHook		hooks;
		SkllConsole		&console;
	// ================================
	// Private vars
	// ================================
	private:
		int 		_epfd;
		int			_reserved_fds;
		int 		_timeout;
		int 		_queue;
		bool 		_running;
		uint16_t	_port;
		std::string	_address;

	std::map<std::string, SkllProtocol*> _protocols;

	// ================================
	// Public funcs
	// ================================
	public:
		~SkllNetwork();
		SkllNetwork(const std::string &port);
		SkllNetwork(const SkllNetwork& other);
		SkllNetwork&	operator=(const SkllNetwork& other);

		int		run();
		void	stop();
		void	clean();
		void	handle();
		void	send();
		
	// ================================
	// Private funcs
	// ================================
	private:
		uint16_t 	_check_port(const std::string &port);
		void		_update_reserved_fds(int delta);
		int			_init_socket();
		int			_init_limit(int max);
		int			_init_reserved(int reserved);

	// ================================
	// Get / Set
	// ================================
	public:
		SkllNetwork	&set_port(const std::string& port);
		SkllNetwork	&set_address(const std::string& address);
		SkllNetwork	&set_max_clients(int max);
		SkllNetwork	&set_reserved_fds(int reserved);
		SkllNetwork	&set_timeout(int seconds);
		SkllNetwork	&set_queue(int backlog);
		SkllNetwork	&set_protocol(const std::string &crlf, bool binary, int protocol, int buffer_size = 512);

		std::string &get_address();
		uint16_t	&get_port();
		int			&get_max_client();
		int			&get_reserved_fds();
		int			&get_timeout();
		int			&get_queue();
};