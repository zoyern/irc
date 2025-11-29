/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllSender.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 19:53:40 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 19:53:40 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllClient.hpp>
#include <sstream>
#include <string>
#include <set>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SENDER - Buffered message builder with << operator                        */
/*   • Accumulates data via <<                                                 */
/*   • Auto-flush on destruction (RAII)                                        */
/*   • Configurable delimiter                                                  */
/*   • Supports both TCP (send) and UDP (sendto)                               */
/*                                                                             */
/*   USAGE:                                                                    */
/*     SkllSender(fd, "\r\n") << "PRIVMSG " << target << " :" << msg;          */
/*     SkllSender(client, "\r\n") << "UDP response";  // UDP with sendto       */
/*     // Auto-flushed when sender goes out of scope                           */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllSender {
public:
	/* TCP send - uses send(fd, ...) */
	SkllSender(fd_t fd, const std::string &delim = "\r\n");
	
	/* UDP send - uses sendto(fd, ..., client->udp_addr()) */
	SkllSender(SkllClient *client, const std::string &delim = "\r\n");
	
	~SkllSender();  /* Auto-flush */

	/* Append data */
	template <typename T>
	SkllSender &			operator<<(const T &val);

	SkllSender &			operator<<(const SkllSpan &span);
	SkllSender &			operator<<(const char *str);

	/* Control */
	void					flush();
	void					clear();
	void					set_delim(const std::string &d);

private:
	fd_t					_fd;
	SkllClient *			_client;  /* For UDP sendto */
	std::string				_delim;
	std::string				_buf;
	bool					_flushed;

public:
	/* Copy constructor - transfers ownership (source becomes flushed) */
	SkllSender(const SkllSender &other);

private:
	SkllSender &operator=(const SkllSender &);
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   BROADCAST - Send to multiple FDs                                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllBroadcast {
public:
	SkllBroadcast(const std::string &delim = "\r\n");
	~SkllBroadcast();

	/* Target management */
	SkllBroadcast &			to(fd_t fd);
	SkllBroadcast &			to(const std::set<fd_t> &fds);
	SkllBroadcast &			except(fd_t fd);

	/* Append data */
	template <typename T>
	SkllBroadcast &			operator<<(const T &val);

	SkllBroadcast &			operator<<(const SkllSpan &span);
	SkllBroadcast &			operator<<(const char *str);

	/* Control - call flush() to send */
	void					flush();
	void					clear();

private:
	std::set<fd_t>			_targets;
	fd_t					_except;
	std::string				_delim;
	std::ostringstream		_buf;
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   TEMPLATE IMPLEMENTATIONS                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

template <typename T>
SkllSender &SkllSender::operator<<(const T &val) {
	std::ostringstream oss;
	oss << val;
	_buf += oss.str();
	return *this;
}

template <typename T>
SkllBroadcast &SkllBroadcast::operator<<(const T &val) {
	_buf << val;
	return *this;
}
