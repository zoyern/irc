/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:17:56 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:17:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Irc.hpp"

class Client;
class Server
{
public:
	Server(void);
	Server(const std::string password, SkllClient &client);
	Channel *getOneChannel(std::string name);
	const std::vector<Channel> &getChannels() const;
	void addChannel(Channel &channel);
	void deleteChannel(std::string name);
	Client *getOneClient(int fd);
	Client *getOneClient(std::string nick);
	const std::vector<Client> &getClients() const;
	void addClient(Client &client);
	void deleteClient(int fd);
	bool checkPassword(std::string pass);
	void Server::broadcastClientQuitMessage(std::string nick, std::map<int, Client> &clientTobroadcast, Channel channel);
	~Server();
	static void on_server_start(void *event, void *user_data);
	static void on_client_connect(void *event, void *user_data);
	// static void on_client_disconnect(void *event, void *user_data);

private:
	std::string _password;
	std::string _name;
	std::vector<Channel> _channels;
	std::vector<Client> _clients;
	SkllClient *_client;
};
