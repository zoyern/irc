/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook_channel.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:32:21 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:32:21 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell.hpp>
#include <Sockell/SkllConsole.hpp>

void	on_channel_start() { SkllConsole::instance().info("Channel") << "SkllServer started on "; }
void	on_channel_update() {}
void	on_channel_shutdown() { SkllConsole::instance().info("Channel") << "SkllServer shutting down."; }
void	on_channel_connect() { SkllConsole::instance().info("Channel") << "Client connected: fd="; }
void	on_channel_disconnect() { SkllConsole::instance().info("Channel") << "Client disconnected: fd=";}
void	on_channel_error() { SkllConsole::instance().info("Channel") << "Client on_channel_error: fd=";}
void	on_channel_timeout() { SkllConsole::instance().info("Channel") << "Client on_channel_timeout: fd=";}
void	on_channel_recv() { SkllConsole::instance().info("Channel") << "Received: "; }
void	on_channel_send() { SkllConsole::instance().info("Channel") << "Send: "; }

void	handle_kick() { SkllConsole::instance().info("Channel") << "handle_kick: "; }
void	handle_invite() { SkllConsole::instance().info("Channel") << "handle_invite: "; }
void	handle_topic() { SkllConsole::instance().info("Channel") << "handle_topic: "; }
void	handle_mode() { SkllConsole::instance().info("Channel") << "handle_mode: "; }
