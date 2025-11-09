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

#include <Sockell.hpp>
#include <Sockell/SkllErrors.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllConsole.hpp>
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllChannel.hpp>

class SkllErrors;
class SkllHook;
class SkllNetwork;
class SkllProtocol;
class SkllConsole;
class SkllClient;
class SkllChannel;

#define SKLL_NAME		"irc"
#define SKLL_MSG		"Welcome to the IRC SkllServer!"

// ================================
// Sockell Server Class
// ================================
class SkllServer {
	// ================================
	// Public vars
	// ================================
	public:
		SkllErrors		errors;
		SkllNetwork		network;
		SkllProtocol	protocol;
		SkllHook		hooks;
		SkllConsole		&console;
	// ================================
	// Private vars
	// ================================
	private:
		int 		_fd;
		bool 		_running;

		std::string	_password;
		std::string	_name;
		std::string	_connexion_msg;

		SkllChannel							*_default_channel;
		std::map<std::string, SkllChannel*>	_channels;
		std::map<int, SkllClient*>			_clients;

	// ================================
	// Public funcs
	// ================================
	public:
	    ~SkllServer();
		SkllServer(const std::string &port, const std::string &password);
		SkllServer(const SkllServer& other);
		SkllServer&	operator=(const SkllServer& other);

		int		run();
		void	stop();
		void	clean();

		SkllChannel& channel(const std::string &name, bool is_default = false);
	// ================================
	// Private funcs
	// ================================
	private:
	// ================================
	// Get / Set
	// ================================
	public:
		SkllServer	&set_password(const std::string& pass);
		SkllServer	&set_name(const std::string& name);
		SkllServer	&set_connexion_msg(const std::string &msg);
		SkllServer	&set_client();
		SkllServer	&set_Channel();

		std::string	&get_password();
		std::string	&get_name();
		std::string &get_connexion_msg();
		SkllClient	&get_client();
		SkllChannel	&get_Channel();
};
