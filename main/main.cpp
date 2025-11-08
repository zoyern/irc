/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:31:36 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:31:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

void start(Server &server, void* data) {
	(void)data;
	(void)server;
	Console::instance().info("Server") << "Server started.";
}

void update(Server &server, void* data) {
	(void)data;
	(void)server;
}

void shutdown(Server &server, void* data) {
	(void)data;
	(void)server;
	Console::instance().info("Server") << "Server shutting down.";
}

void on_connect(Server &server, void* data) {
	(void)data;
	(void)server;
	Console::instance().info("Server") << "New client connected.";
}

void on_disconnect(Server &server, void* data) {
	(void)data;
	(void)server;
	Console::instance().info("Server") << "Client disconnected.";
}

int main(int argc, char const **argv)
{
	if (argc != 3) return (std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl, 1);
	try {
		Server srv(argv[1], argv[2]);
		srv.name("irc")
			.address("0.0.0.0")
			.connexion_msg(MSG)
			.max_clients(MAX_CLIENTS)
			.reserved_fds(RESERVED_FD)
			.timeout(TIMEOUT)
			.queue(QUEUE)
			.hook(ON_START, &start, NULL)
			.hook(ON_UPDATE, &update, NULL)
			.hook(ON_SHUTDOWN, &shutdown, NULL)
			.hook(ON_CONNECT, &on_connect, NULL)
			.hook(ON_DISCONNECT, &on_disconnect, NULL);

		srv.channel(CHANNEL_NAME, CHANNEL_DEFAULT).password("").op(CHANNEL_OPERATOR).size(CHANNEL_SIZE)
			.invite_only(CHANNEL_INV_ONLY).restricted_topic(CHANNEL_REST_TOPIC).topic(CHANNEL_TOPIC);
		return (srv.run());
	}
	catch (std::exception &e){return (std::cerr << "Error: " << e.what() << std::endl,1);}
	return (0);
}
