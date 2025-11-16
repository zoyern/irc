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

void	on_channel_start() { std::cout << "Channel SkllServer started on " << std::endl; }
void	on_channel_update() {}
void	on_channel_shutdown() { std::cout << "Channel SkllServer shutting down." << std::endl; }
void	on_channel_connect() { std::cout << "Channel Client connected: fd=" << std::endl; }
void	on_channel_disconnect() { std::cout << "Channel Client disconnected: fd=" << std::endl;}
void	on_channel_error() { std::cout << "Channel Client on_channel_error: fd=" << std::endl;}
void	on_channel_timeout() { std::cout << "Channel Client on_channel_timeout: fd=" << std::endl;}
void	on_channel_recv() { std::cout << "Channel Received: " << std::endl; }
void	on_channel_send() { std::cout << "Channel Send: " << std::endl; }

void	handle_kick() { std::cout << "Channel handle_kick: " << std::endl; }
void	handle_invite() { std::cout << "Channel handle_invite: " << std::endl; }
void	handle_topic() { std::cout << "Channel handle_topic: " << std::endl; }
void	handle_mode() { std::cout << "Channel handle_mode: " << std::endl; }
