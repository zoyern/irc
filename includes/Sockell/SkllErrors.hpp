/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllErrors.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 02:37:35 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 02:37:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <exception>
#include <string>

class SkllException : public std::exception {
protected:
    std::string _msg;
public:
    SkllException(const std::string& msg);
    virtual ~SkllException() throw();
    virtual const char* what() const throw();
};

class SkllErrorSocket : public SkllException {
public:
    SkllErrorSocket(const std::string& msg);
};

class SkllErrorBind : public SkllException {
public:
    SkllErrorBind(const std::string& msg);
};

class SkllErrorListen : public SkllException {
public:
    SkllErrorListen(const std::string& msg);
};

class SkllErrorEpoll : public SkllException {
public:
    SkllErrorEpoll(const std::string& msg);
};

class SkllErrorFull : public SkllException {
public:
    SkllErrorFull(const std::string& msg);
};