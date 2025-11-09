/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllNetwork_private.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 22:18:11 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 22:18:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllNetwork.hpp>

uint16_t SkllNetwork::_check_port(const std::string &port) {
	std::istringstream iss(port);
	int port_num;
	if (!(iss >> port_num) || port_num > 65535 || port_num < 1)
		throw (std::invalid_argument("Invalid port"));
	return(static_cast<uint16_t>(port_num));
}

void SkllNetwork::_update_reserved_fds(int delta) { (void)delta; }

int SkllNetwork::_init_socket() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	fd < 0 ? throw(std::runtime_error("socket() failed")) : fd;

	return (fd);
}

int SkllNetwork::_init_limit(int max) { (void)max; return (0); }
int SkllNetwork::_init_reserved(int reserved) { (void)reserved; return (0); }