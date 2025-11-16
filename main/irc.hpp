/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:33:47 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:33:47 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <Sockell.hpp>
#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllNetwork.hpp>

// ================================
// Server
// ================================
void	on_start();
void	on_update();
void	on_shutdown();
void	on_connect();
void	on_disconnect();
void	on_error();
void	on_timeout();
void	on_recv();
void	on_send();

void	handle_nick();
void	handle_join();
void	handle_privmsg();


// ================================
// Channel
// ================================
void	on_channel_start();
void	on_channel_update();
void	on_channel_shutdown();
void	on_channel_connect();
void	on_channel_disconnect();
void	on_channel_error();
void	on_channel_timeout();
void	on_channel_recv();
void	on_channel_send();

void	handle_kick();
void	handle_invite();
void	handle_topic() ;
void	handle_mode();
