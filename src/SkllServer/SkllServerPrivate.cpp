/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServerPrivate.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:17:12 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:17:12 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllServer.hpp>

SkllServer::SkllServer(const SkllServer&) {}
SkllServer	&SkllServer::operator=(const SkllServer&) { return (*this); }

void SkllServer::trigger_event(int type)
{
	SkllEvent event;
	event.type = type;
	event.server = this;
	for (NetworksMapIt it = _nets.begin(); it != _nets.end(); ++it) {
		event.network = it->second; 
		_hook.trigger(type, &event);
	}
}
