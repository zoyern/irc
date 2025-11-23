/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllEvent.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:51:10 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:51:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllEvent.hpp>
#include <cstddef>

SkllEvent::SkllEvent()
	: type(0)
	, server(NULL)
	, network(NULL)
	, protocol(NULL)
	, client(NULL)
	, message(NULL)
	, fd(-1)
	, signal_num(0)
	, timer_id(0)
	, error_code(0)
	, error_msg(NULL) {}