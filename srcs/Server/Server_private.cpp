/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_private.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 21:36:33 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 21:36:33 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

uint16_t Server::check_port(const std::string &port) {
	std::istringstream iss(port);
	int port_num;
	if (!(iss >> port_num) || port_num > 65535 || port_num < 1)
		throw (std::invalid_argument("Invalid port"));
	return(static_cast<uint16_t>(port_num));
}

void Server::_update_reserved_fds(int delta) { _used_reserved_fds += delta;	_max_clients = _calculate_max_clients(MAX_CLIENTS, _reserved_fds); }

void Server::_init_socket() {}
void Server::_init_epoll() {}
void Server::_init_limits() {}
