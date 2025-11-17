/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllMessage.tpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 03:45:30 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 03:45:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllMessage.hpp>

template<typename T> T			SkllMessage::to() const { return (T());}
template<> inline std::string	SkllMessage::to<std::string>() const { return (std::string(_buf.begin(), _buf.end())); }
template<> inline int			SkllMessage::to<int>() const { if (_buf.size() < sizeof(int)) return (0); int v; std::memcpy(&v, &_buf[0], sizeof(int)); return (v); }
template<> inline uint32_t		SkllMessage::to<uint32_t>() const { if (_buf.size() < sizeof(uint32_t)) return 0; uint32_t v; std::memcpy(&v, &_buf[0], sizeof(uint32_t)); return (ntohl(v)); }
