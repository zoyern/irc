/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllServer_public.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 21:36:35 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 21:36:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllServer.hpp>

// ================================
// Setters
// ================================
SkllServer	&SkllServer::set_password(const std::string& pass) { _password = pass; return (*this); }
SkllServer	&SkllServer::set_name(const std::string& name) { _name = name; return (*this); }
SkllServer	&SkllServer::set_connexion_msg(const std::string &msg) { _connexion_msg = msg; return (*this); }

// ================================
// Getters
// ================================
std::string	&SkllServer::get_password() { return (_password);}
std::string	&SkllServer::get_name() { return (_name);}
std::string &SkllServer::get_connexion_msg() { return (_connexion_msg);}
