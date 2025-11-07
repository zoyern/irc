/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lobby.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:37:00 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:37:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../irc.hpp"

class Channel
{
	public:
		Channel(void);
		Channel(std::string name);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		Channel& name(const std::string& name);
		Channel& password(const std::string& pass);
		Channel& op(const std::string& name);
		Channel& size(const std::string& address);
		Channel& invite_only(const std::string &msg);
		Channel& restricted_topic (int max);
		Channel& topic(int reserved);
		~Channel(void);
	private:
		std::string _name;
		std::string _password;
		int _op;
		int _size;
		bool _invite_only;
		bool _restricted_topic;
		std::string _topic;
};

// SERVERR -> creation d'un client , creation d'un channel
// main -> commandes globales, commandes de channel que tu veux

// .hook(ON_START, &start, NULL)
//     .hook(ON_UPDATE, &update, NULL)
// .hook(ON_SHUTDOWN, &shutdown, NULL)
// .hook(ON_CONNECT, &on_connect, NULL)
// .hook(ON_DISCONNECT, &on_disconnect, NULL)

// Channel &Server::channel(const std::string &name, bool is_default) {
//     std::pair<std::map<std::string, Channel>::iterator, bool> channel =
//     _channels.insert(std::pair<std::string, Channel>(name, Channel(name)));
//     if (is_default) { console.log(INFO, CHANNEL_SET_DEFAULT + name); _default_channel = channel.first->second; }
//     channel.second ? console.log(INFO, CHANNEL_CREATED + name) : console.log(INFO, CHANNEL_EXISTS + name);
//     return (channel.first->second);
// }

// Channel(name)
// std::string _name
// std::string _password
// int op; // operateur du channel a voir si on fais un truc en mode il peux y avoir des privilége different donc plusieurs operateur avec des privilége different bonnus
// int size;
// bool invite_only;
// bool restricted_topic

// Channel&    name(const std::string& port);
// Channel&    password(const std::string& pass);
// Channel&    op(const std::string& name);
// Channel& size(const std::string& address);
// Channel& invite_only(const std::string &msg);
// Channel& restricted_topic (int max);
// Channel& topic(int reserved);
