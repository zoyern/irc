/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 18:14:10 by marvin            #+#    #+#             */
/*   Updated: 2025/11/28 18:14:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllException.hpp>

const char *SkllException::SocketException::what() const throw()		{ return SKLL_EX_SOCKET; }
const char *SkllException::BindException::what() const throw()			{ return SKLL_EX_BIND; }
const char *SkllException::ListenException::what() const throw()		{ return SKLL_EX_LISTEN; }
const char *SkllException::EpollException::what() const throw()		{ return SKLL_EX_EPOLL; }
const char *SkllException::ConnectException::what() const throw()		{ return SKLL_EX_CONNECT; }
const char *SkllException::ConfigException::what() const throw()		{ return SKLL_EX_CONFIG; }
const char *SkllException::PortException::what() const throw()			{ return SKLL_EX_PORT; }
const char *SkllException::ProtocolException::what() const throw()		{ return SKLL_EX_PROTO; }
const char *SkllException::RouteException::what() const throw()		{ return SKLL_EX_ROUTE; }
const char *SkllException::NetworkException::what() const throw()		{ return SKLL_EX_NETWORK; }
const char *SkllException::TimeoutException::what() const throw()		{ return SKLL_EX_TIMEOUT; }
const char *SkllException::AuthException::what() const throw()			{ return SKLL_EX_AUTH; }
const char *SkllException::PermissionException::what() const throw()	{ return SKLL_EX_PERM; }
const char *SkllException::NotFoundException::what() const throw()		{ return SKLL_EX_NOTFOUND; }
const char *SkllException::LimitException::what() const throw()		{ return SKLL_EX_LIMIT; }
const char *SkllException::ParseException::what() const throw()		{ return SKLL_EX_PARSE; }
