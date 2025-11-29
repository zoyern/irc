/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockell.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:35:03 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:35:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SOCKELL - High-Performance Networking Library                             */
/*                                                                             */
/*   A lightweight, C++98-compliant networking library designed for            */
/*   building high-performance servers with fluent API.                        */
/*                                                                             */
/*   Architecture:                                                             */
/*     Server → Network[] → Protocol[] → Client[]                              */
/*                       ↘ Router (fluent routing with variables)              */
/*                       ↘ Channel[] (groups with custom permissions)          */
/*                                                                             */
/*   Features:                                                                 */
/*     • TCP + UDP support (send/recv, sendto/recvfrom)                        */
/*     • IPv4 + IPv6 + Dual-stack support                                      */
/*     • epoll per network (scalable I/O)                                      */
/*     • Fluent API with << operators                                          */
/*     • Template-based user data (no void* casts)                             */
/*     • Zero-copy message parsing                                             */
/*     • Unlimited callbacks, routes, clients, channels                        */
/*     • Built-in rate limiting (DDoS protection)                              */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* Core */
#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllException.hpp>
#include <Sockell/SkllLog.hpp>
#include <Sockell/SkllBuffer.hpp>
#include <Sockell/SkllMessage.hpp>
#include <Sockell/SkllSender.hpp>

/* Routing &Hooks */
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllRouter.hpp>

/* Connection Management */
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllChannel.hpp>
#include <Sockell/SkllProtocol.hpp>
#include <Sockell/SkllNetwork.hpp>

/* Server &Signals */
#include <Sockell/SkllSignals.hpp>
#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllServer.hpp>
