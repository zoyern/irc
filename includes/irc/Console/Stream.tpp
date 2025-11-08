/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Stream.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 02:13:03 by marvin            #+#    #+#             */
/*   Updated: 2025/11/08 02:13:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <irc.hpp>

template<typename T>
Console::Stream	&Console::Stream::operator<<(const T& value) { _buffer << value; return (*this); }
