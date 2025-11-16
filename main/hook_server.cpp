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
#include <Sockell/SkllServer.hpp>

void	on_start() { std::cout << "main SkllServer started on " << std::endl; }
void	on_update() {}
void	on_shutdown() { std::cout << "main SkllServer shutting down." << std::endl; }
void	on_connect() { std::cout << "main Client connected: fd=" << std::endl; }
void	on_disconnect() { std::cout << "main Client disconnected: fd=" << std::endl;}
void	on_error() { std::cout << "main Client on_error: fd=" << std::endl;}
void	on_timeout() { std::cout << "main Client on_timeout: fd=" << std::endl;}
void	on_recv(/*SkllServer server, bitedef msg, void *data*/) { 
		std::cout << "main Received: " << std::endl; 
		/*	if (msg == ("NICK"))
		.on("NICK", &handle_nick, NULL) // header perso
		.on("JOIN", &handle_join, NULL)
		.on(0x01, &handle_join, NULL)
		.on("PRIVMSG", &handle_privmsg, NULL);
	*/
	}

void	on_send() { std::cout << "main Send: " << std::endl; }

void	handle_nick() { std::cout << "main handle_nick: " << std::endl; }
void	handle_join() { std::cout << "main handle_join: " << std::endl; }
void	handle_privmsg() { std::cout << "main handle_privmsg: " << std::endl; }
