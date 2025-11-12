/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook_SkllServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:32:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:32:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell.hpp>
#include <Sockell/SkllConsole.hpp>
#include <Sockell/SkllServer.hpp>

void	on_start() { SkllConsole::instance().info("main") << "SkllServer started on "; }
void	on_update() {}
void	on_shutdown() { SkllConsole::instance().info("main") << "SkllServer shutting down."; }
void	on_connect() { SkllConsole::instance().info("main") << "Client connected: fd="; }
void	on_disconnect() { SkllConsole::instance().info("main") << "Client disconnected: fd=";}
void	on_error() { SkllConsole::instance().info("main") << "Client on_error: fd=";}
void	on_timeout() { SkllConsole::instance().info("main") << "Client on_timeout: fd=";}
void	on_recv(/*SkllServer server, bitedef msg, void *data*/) { 
		SkllConsole::instance().info("main") << "Received: "; 
		/*	if (msg == ("NICK"))
		.on("NICK", &handle_nick, NULL) // header perso
		.on("JOIN", &handle_join, NULL)
		.on(0x01, &handle_join, NULL)
		.on("PRIVMSG", &handle_privmsg, NULL);
	*/
	}

void	on_send() { SkllConsole::instance().info("main") << "Send: "; }

void	handle_nick() { SkllConsole::instance().info("main") << "handle_nick: "; }
void	handle_join() { SkllConsole::instance().info("main") << "handle_join: "; }
void	handle_privmsg() { SkllConsole::instance().info("main") << "handle_privmsg: "; }
