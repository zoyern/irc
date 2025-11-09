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

void	on_channel_start() { srv.console.info("Channel") << "SkllServer started on "; }
void	on_channel_update() {}
void	on_channel_shutdown() { srv.console.info("Channel") << "SkllServer shutting down."; }
void	on_channel_connect() { srv.console.info("Channel") << "Client connected: fd="; }
void	on_channel_disconnect() { srv.console.info("Channel") << "Client disconnected: fd=";}
void	on_channel_error() { srv.console.info("Channel") << "Client on_channel_error: fd=";}
void	on_channel_timeout() { srv.console.info("Channel") << "Client on_channel_timeout: fd=";}
void	on_channel_recv() { srv.console.info("Channel") << "Received: "; }
void	on_channel_send() { srv.console.info("Channel") << "Send: "; }

void	handle_kick() { srv.console.info("Channel") << "handle_kick: "; }
void	handle_invite() { srv.console.info("Channel") << "handle_invite: "; }
void	handle_topic() { srv.console.info("Channel") << "handle_topic: "; }
void	handle_mode() { srv.console.info("Channel") << "handle_mode: "; }
