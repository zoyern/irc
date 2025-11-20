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

#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllNetwork.hpp>
#include <Sockell/SkllProtocol.hpp>
#include "IRCServer.hpp"
#include <cstdlib>
#include <iostream>

enum ControlType
{
	IRC_END  = 1 << 0,	  // 0b0001
	IRC_END2 = 1 << 1,  // 0b0010
};

void on_connect(void *lib, void *user)
{
	if (!lib || !user)
		return;
	int *fd = static_cast<int *>(lib);
	IRCServer *irc = static_cast<IRCServer *>(user);
	irc->on_connect(*fd);
}

void on_recv(void *lib, void *user)
{
	(void)lib;
	if (!user)
		return;
	IRCServer *irc = static_cast<IRCServer *>(user);
	irc->on_recv();
}

int main(int argc, char const **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>\n";
		return 1;
	}

	try
	{
		IRCServer irc(argv[2]);

		SkllProtocol protocol_irc("irc", "0.0.0.0", std::atoi(argv[1]), SKLL_TCP | SKLL_IPV4);
		SkllNetwork network("net", 100, 128);
		SkllServer server(IRC_MAX_CLIENTS, 10);

		server.networks(network)
			.listen(protocol_irc)

		protocol_irc
			.reuse_addr(true)
			.reuse_port(false)
			.linger(5652)
			.timout_send(0)
			.timout_recv(0)
			.recv_timeout(0)
			.buffer(0)
			.chunk(16)
			.on(ON_CONNECT, on_connect)
			.on(ON_RECV, on_recv)
			.EOM(IRC_END, "\r\n") // End Of Message
			.EOM(IRC_END2, "\n");

		protocol.ctrl(IRC_END | IRC_END2)
			.route("USER ", handle_user) //  <username>> <password>
			.route("NICK  ", handle_nick) // <nickname>
			.route("JOIN ", handle_user); //<channel>{,<channel>} [<key>{,<key>}]

		protocol.ctrl(IRC_END | IRC_END2)
			.route("PRIVMSG", handle_privmsg)
				.match("#", handle_channel)			 // ← Si '#'
				.match("@", handle_bot);				 // ← Ou si '@'

		protocol.ctrl(IRC_END | IRC_END2)
			.route("KICK ", handle_nick) // <channel> <user> [<comment>]
			.route("INVITE ", handle_user) // <nickname> <channel>
			.route("TOPIC", handle_user) // <channel> [<topic>]
			.route("QUIT", handle_quit); // [<Quit message>]

		// Middleware : callback principal + sous-callbacks
		protocol.ctrl(IRC_END | IRC_END2)
			.route("MODE", handle_mode)
				.match("i", handle_join_channel) // i - invite-only channel flag;
				.match("t", handle_join_channel) // t - topic settable by channel operator only flag;
				.match("k", handle_join_channel) // k - set a channel key (password).
				.match("o", handle_join_channel) // o - give/take channel operator privileges;
				.match("l", handle_join_channel);// l - set the user limit to channel;


		return server.run();
	}
	catch (SkllException &e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
}