/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllExeption.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 19:51:09 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 19:51:09 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <exception>
#include <string>
#include <cstring>
#include <cerrno>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   EXCEPTION MESSAGES                                                        */
/* ═══════════════════════════════════════════════════════════════════════════ */

#ifndef SKLL_EX_MSG
# define SKLL_EX_SOCKET			"Socket creation failed"
# define SKLL_EX_BIND			"Cannot bind to address"
# define SKLL_EX_LISTEN			"Cannot start listening"
# define SKLL_EX_EPOLL			"Epoll operation failed"
# define SKLL_EX_CONNECT		"Connection failed"
# define SKLL_EX_CONFIG			"Invalid configuration"
# define SKLL_EX_PORT			"Invalid port number"
# define SKLL_EX_PROTO			"Invalid protocol"
# define SKLL_EX_ROUTE			"Invalid route"
# define SKLL_EX_NETWORK		"Network error"
# define SKLL_EX_TIMEOUT		"Operation timed out"
# define SKLL_EX_AUTH			"Authentication failed"
# define SKLL_EX_PERM			"Permission denied"
# define SKLL_EX_NOTFOUND		"Resource not found"
# define SKLL_EX_LIMIT			"Limit exceeded"
# define SKLL_EX_PARSE			"Parse error"
# define SKLL_EX_INTERNAL		"Internal error"
#endif

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLL EXCEPTION - Base exception class (style 42)                          */
/*                                                                             */
/*   Usage:                                                                    */
/*     throw SkllException::SocketException("context");                        */
/*     throw SkllException::BindException();                                   */
/*     throw SkllException::AuthException();                                   */
/*                                                                             */
/*   Catch:                                                                    */
/*     catch (const std::exception &e) { std::cerr << e.what(); }             */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllException {
public:
	/* Socket creation/configuration errors */
	class SocketException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Address binding errors */
	class BindException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Listen queue errors */
	class ListenException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Epoll operation errors */
	class EpollException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Connection/accept errors */
	class ConnectException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Configuration errors */
	class ConfigException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Invalid port number */
	class PortException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Protocol errors */
	class ProtocolException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Route errors */
	class RouteException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Network errors */
	class NetworkException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Timeout errors */
	class TimeoutException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Authentication errors */
	class AuthException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Permission denied */
	class PermissionException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Resource not found */
	class NotFoundException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Limit exceeded */
	class LimitException : public std::exception {
	public:
		virtual const char *what() const throw();
	};

	/* Parse error */
	class ParseException : public std::exception {
	public:
		virtual const char *what() const throw();
	};
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   LEGACY ALIASES (backward compatibility)                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

typedef SkllException::SocketException		SkllSocketException;
typedef SkllException::BindException		SkllBindException;
typedef SkllException::ListenException		SkllListenException;
typedef SkllException::EpollException		SkllEpollException;
typedef SkllException::ConnectException		SkllConnectionException;
typedef SkllException::ConfigException		SkllConfigException;
typedef SkllException::PortException		SkllPortException;
typedef SkllException::ProtocolException	SkllProtocolException;
typedef SkllException::RouteException		SkllRouteException;
typedef SkllException::NetworkException		SkllNetworkException;
typedef SkllException::TimeoutException		SkllTimeoutException;
typedef SkllException::AuthException		SkllAuthException;
typedef SkllException::PermissionException	SkllPermissionException;
typedef SkllException::NotFoundException	SkllNotFoundException;
typedef SkllException::LimitException		SkllLimitException;
typedef SkllException::ParseException		SkllParseException;
