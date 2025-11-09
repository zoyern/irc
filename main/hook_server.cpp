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

void	on_start() { srv.console.info("main") << "SkllServer started on "; }
void	on_update() {}
void	on_shutdown() { srv.console.info("main") << "SkllServer shutting down."; }
void	on_connect() { srv.console.info("main") << "Client connected: fd="; }
void	on_disconnect() { srv.console.info("main") << "Client disconnected: fd=";}
void	on_error() { srv.console.info("main") << "Client on_error: fd=";}
void	on_timeout() { srv.console.info("main") << "Client on_timeout: fd=";}
void	on_recv() { srv.console.info("main") << "Received: "; }
void	on_send() { srv.console.info("main") << "Send: "; }

void	handle_nick() { srv.console.info("main") << "handle_nick: "; }
void	handle_join() { srv.console.info("main") << "handle_join: "; }
void	handle_privmsg() { srv.console.info("main") << "handle_privmsg: "; }
