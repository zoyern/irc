/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllErrors.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 16:58:55 by marvin            #+#    #+#             */
/*   Updated: 2025/11/16 16:58:55 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllErrors.hpp>

SkllException::SkllException(const std::string& msg) : _msg(msg) {}
SkllException::~SkllException() throw() {}
const char* SkllException::what() const throw() { return _msg.c_str(); }

SkllErrorSocket::SkllErrorSocket(const std::string& m) : SkllException(m) {}
SkllErrorBind::SkllErrorBind(const std::string& m) : SkllException(m) {}
SkllErrorListen::SkllErrorListen(const std::string& m) : SkllException(m) {}
SkllErrorEpoll::SkllErrorEpoll(const std::string& m) : SkllException(m) {}