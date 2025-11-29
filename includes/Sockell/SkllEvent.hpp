/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllEvent.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:49:20 by marvin            #+#    #+#             */
/*   Updated: 2025/11/13 17:49:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllMessage.hpp>
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllSender.hpp>
#include <Sockell/SkllNetwork.hpp>

class SkllServer;
class SkllNetwork;
class SkllChannel;
class SkllChannels;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   EVENT - Fluent interface for handlers                                     */
/*                                                                             */
/*   DESIGN PHILOSOPHY:                                                        */
/*     • Event gives access to library features, NOT user data management      */
/*     • User manages their own fd -> UserData mapping (std::map)              */
/*     • NO casts, NO templates, NO macros                                     */
/*     • Fluent send() with << operator                                        */
/*     • Clean, readable API for both juniors and seniors                      */
/*                                                                             */
/*   USAGE:                                                                    */
/*     fd_t connection = event.fd();                                           */
/*     std::string client_ip = event.ip();                                     */
/*     SkllSpan command = event.command();                                     */
/*     event.send() << "Hello " << name;                                       */
/*     event.send(other_fd) << "Message to other client";                      */
/*     SkllChannel *channel = event.channel("#general");                      */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllEvent {
public:
	SkllEvent();
	~SkllEvent();

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   CONNECTION INFO - Read-only access to client connection               */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	fd_t					fd() const;                 /* Client file descriptor   */
	const std::string &		ip() const;                 /* Client IP address        */
	int						event_type() const;         /* Event type (SKLL_EV_*)   */
	int						signal_number() const;      /* Signal number if SIGNAL  */
	bool					is_udp() const;             /* True if UDP connection   */

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   MESSAGE ACCESS - Parsed IRC message                                   */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	SkllMessage &			message();                  /* Full message object      */
	const SkllMessage &		message() const;
	SkllSpan				command() const;            /* Command (NICK, JOIN...)  */
	size_t					arg_count() const;          /* Number of arguments      */
	SkllSpan				argument(size_t index) const; /* Argument by index      */
	SkllSpan				trailing() const;           /* Trailing text after :    */

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   SEND API - Fluent message sending                                     */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	SkllSender				send();                     /* Send to current fd       */
	SkllSender				send(fd_t target_fd);       /* Send to specific fd      */
	void					set_line_ending(const std::string &ending);
	const std::string &		line_ending() const;

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   CHANNEL ACCESS - Channel management                                   */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	SkllChannels &			channels();                 /* All channels manager     */
	SkllChannel *			channel(const std::string &name); /* Get channel by name */

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   SERVER/NETWORK ACCESS - Library components                            */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	SkllServer *			server();                   /* Main server instance     */
	SkllNetwork *			network();                  /* Network manager          */
	const SkllServer *		server() const;
	const SkllNetwork *		network() const;

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   INTERNAL - Library use only (prefixed with set_)                      */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	void					set_event_type(int type);
	void					set_fd(fd_t file_descriptor);
	void					set_server(SkllServer *srv);
	void					set_network(SkllNetwork *net);
	void					set_client(SkllClient *cli);
	void					set_signal_number(int sig);
	void					set_udp(bool udp);
	void					reset();
	
	/* Internal client access for library */
	SkllClient *			internal_client();
	const SkllClient *		internal_client() const;

private:
	int						_event_type;
	fd_t					_fd;
	int						_signal;
	bool					_udp;
	std::string				_line_ending;
	SkllServer *			_server;
	SkllNetwork *			_network;
	SkllClient *			_client;
	SkllMessage				_message;

	SkllEvent(const SkllEvent &);
	SkllEvent &operator=(const SkllEvent &);
};
