/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllNetworking.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 18:53:28 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 18:53:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllNetwork.hpp>

// ================================
// Setters
// ================================
SkllNetwork	&SkllNetwork::set_address(const std::string &address) { _address = address; return (*this); }
SkllNetwork	&SkllNetwork::set_port(const std::string& port) { _port = _check_port(port); return (*this); }
SkllNetwork	&SkllNetwork::set_max_clients(int max) { _max_clients = _init_limit(max); return (*this); }
SkllNetwork	&SkllNetwork::set_reserved_fds(int reserved) { _reserved_fds = _init_reserved(reserved); _init_limit(_max_clients - _reserved_fds); return (*this); }
SkllNetwork	&SkllNetwork::set_timeout(int seconds) { _timeout = seconds; return (*this); }
SkllNetwork	&SkllNetwork::set_queue(int backlog) { _queue = backlog; return (*this); }

// ================================
// Getters
// ================================
std::string &SkllNetwork::get_address() { return (_address);}
uint16_t	&SkllNetwork::get_port() { return (_port);}
int			&SkllNetwork::get_max_client() { return (_max_clients);}
int			&SkllNetwork::get_reserved_fds() { return (_reserved_fds);}
int			&SkllNetwork::get_timeout() { return (_timeout);}
int			&SkllNetwork::get_queue() { return (_queue);}
