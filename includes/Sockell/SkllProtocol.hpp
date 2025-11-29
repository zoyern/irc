/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllProtocol.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 15:27:29 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 15:27:29 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <string>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   PROTOCOL - Socket wrapper (TCP, UDP)                                      */
/*   • Wraps socket creation, bind, listen                                     */
/*   • Configurable options (reuseaddr, nodelay, keepalive)                    */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllProtocol {
public:
	SkllProtocol();
	SkllProtocol(int flags, int port);
	~SkllProtocol();

	/* Configuration */
	SkllProtocol &			set_flags(int f);
	SkllProtocol &			set_port(int p);
	SkllProtocol &			set_host(const std::string &h);
	SkllProtocol &			set_reuseaddr(bool v);
	SkllProtocol &			set_nodelay(bool v);
	SkllProtocol &			set_keepalive(bool v);
	SkllProtocol &			set_backlog(int b);

	/* Getters */
	int						flags() const;
	int						port() const;
	const std::string &		host() const;
	bool					reuseaddr() const;
	bool					nodelay() const;
	bool					keepalive() const;
	int						backlog() const;
	fd_t					fd() const;

	/* Type checks */
	bool					is_tcp() const;
	bool					is_udp() const;
	bool					is_ipv4() const;
	bool					is_ipv6() const;

	/* Lifecycle */
	bool					bind();
	void					close();
	bool					bound() const;

private:
	int						_flags;
	int						_port;
	std::string				_host;
	bool					_reuseaddr;
	bool					_nodelay;
	bool					_keepalive;
	int						_backlog;
	fd_t					_fd;

	SkllProtocol(const SkllProtocol &);
	SkllProtocol &operator=(const SkllProtocol &);
};
