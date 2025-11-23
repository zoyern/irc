/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 02:09:32 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 02:09:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockall.hpp>
#include <IRC.hpp>
#include <IRC/IRCServer.hpp>
#include <iostream>
#include <cstdlib>

#define	IRC_MAX_CLIENT		10000
#define	IRC_TIMEOUT 		0
#define	IRC_QUEUE_EVENT		128

int main(int argc, char **argv) {
	if (argc != 3) return (std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl, 1);
	
	int port = std::atoi(argv[1]); std::string password = argv[2];
	if (port < 1024 || port > 65535) return (std::cerr << "Error: Port must be between 1024-65535" << std::endl, 1);
	try {
		IRCServer		irc_server(password);
		SkllServer		server(IRC_MAX_CLIENT);
		SkllProtocol	irc_tcp("irc", "0.0.0.0", port, SKLL_TCP | SKLL_IPV4);
		SkllNetwork		network("irc_network", IRC_TIMEOUT, IRC_QUEUE_EVENT);

		server
			.on(SKLL_ON_START, IRCServer::on_server_start, &irc_server)
			.networks(network)
				.on(SKLL_ON_SIGNAL, on_signal, NULL)
				.listen(irc_tcp)
					.set_reuseaddr(true)
					.set_nodelay(true)
					.set_keepalive(true)
					.set_chunk_size(4096)
					.on(SKLL_ON_CONNECT, IRCServer::on_client_connect, &irc_server)
					.on(SKLL_ON_DISCONNECT, IRCServer::on_client_disconnect, &irc_server)
					.router()
						.delim(IRC_CRLF, "\r\n")
						.delim(IRC_LF, "\n")
					.router()
						.ctrl(IRC_CRLF | IRC_LF)
							.route("PASS ", irc_cmd_pass, &irc_server)
							.route("NICK ", irc_cmd_nick, &irc_server)
							.route("USER ", irc_cmd_user, &irc_server)
							.route("JOIN ", irc_cmd_join, &irc_server)
							.route("PART ", irc_cmd_part, &irc_server)
							.route("TOPIC ", irc_cmd_topic, &irc_server)
							.route("PRIVMSG ", irc_cmd_privmsg, &irc_server)
							.route("MODE ", irc_cmd_mode, &irc_server)
								.match("i", irc_cmd_mode_i, &irc_server)
								.match("t", irc_cmd_mode_t, &irc_server)
								.match("k", irc_cmd_mode_k, &irc_server)
								.match("o", irc_cmd_mode_o, &irc_server)
								.match("l", irc_cmd_mode_l, &irc_server)
							.route("KICK ", irc_cmd_kick, &irc_server)
							.route("INVITE ", irc_cmd_invite, &irc_server)
							.route("PING ", irc_cmd_ping, &irc_server)
							.route("QUIT", irc_cmd_quit, &irc_server);
		return (std::cout << SKLL_GREEN << "\n[" << irc_timestamp() << "] " << SKLL_RESET << "Server ready on port " << SKLL_BOLD << port << SKLL_RESET << std::endl,
			server.run());
	} catch (const std::exception &e)
		{ return (std::cerr << IRC_COLOR_ERROR << "[" << irc_timestamp() << "] Fatal: " << SKLL_RESET << e.what() << std::endl, 1); }
}
