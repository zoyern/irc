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

// ================================
// Server
// ================================
void	on_start(SkllServer &srv, void* data);
void	on_update(SkllServer &srv, void* data);
void	on_shutdown(SkllServer &srv, void* data);
void	on_connect(SkllServer &srv, void* data);
void	on_disconnect(SkllServer &srv, void* data);
void	on_recv(SkllServer &srv, void* data);
void	on_error(SkllServer &srv, void* data);

// ================================
// Channel
// ================================
void	on_channel_start(SkllServer &srv, void* data);
void	on_channel_update(SkllServer &srv, void* data);
void	on_channel_shutdown(SkllServer &srv, void* data);
void	on_channel_connect(SkllServer &srv, void* data);
void	on_channel_disconnect(SkllServer &srv, void* data);
void	on_channel_recv(SkllServer &srv, void* data);
void	on_channel_error(SkllServer &srv, void* data);
