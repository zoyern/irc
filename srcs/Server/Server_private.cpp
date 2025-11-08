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

void Server::_update_reserved_fds(int delta) { (void)delta; }

int Server::_init_socket() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	fd < 0 ? throw(std::runtime_error("socket() failed")) : fd;

	return (fd);
}
Epoll &Server::_init_epoll() {
	try  {_epoll = Epoll(); }
	catch (std::exception &e) { clean(); throw std::runtime_error(e.what()); }
	return (_epoll);
}

int Server::_init_limit(int max) { (void)max; return (0); }
int Server::_init_reserved(int reserved) { (void)reserved; return (0); }