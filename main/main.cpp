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

#include "irc.hpp"

// ================================
// Main
// ================================

int main(int argc, char const **argv)
{
    if (argc != 3)
        return (SkllConsole::instance().error("Usage") << "./ircserv <protocol> [port]", 1);

    try {
        SkllServer srv(argv[1], argv[2], "irc", SKLL_MSG);

        // ================================
        // Configuration générale
        // ================================
        srv.network
			.set_address("0.0.0.0")            // Écoute sur toutes les interfaces
			.set_max_clients(100)              // Limite clients
			.set_reserved_fds(10)              // RéSkllServer FDs
			.set_timeout(30)                   // Timeout en secondes
			.set_queue(128)                    // Queue backlog
			.set_protocol("\r\n", false, SKLL_TCP, 2048);
	
		// ================================
        // Hooks
        // ================================
        srv.hooks
			.on(ON_START, &on_start, NULL)
			.on(ON_UPDATE, &on_update, NULL)
			.on(ON_SHUTDOWN, &on_shutdown, NULL)
			.on(ON_CONNECT, &on_connect, NULL)
			.on(ON_DISCONNECT, &on_disconnect, NULL)
			.on(ON_ERROR, &on_error, NULL)
			.on(ON_TIMEOUT, &on_timeout, NULL)
			.on(ON_RECV, &on_recv, NULL)
			.on(ON_SEND, &on_send, NULL)
			.on("NICK", &handle_nick, NULL)
			.on("JOIN", &handle_join, NULL)
			.on("PRIVMSG", &handle_privmsg, NULL);

        // ================================
        // Channels
        // ================================ // re réflechir a comment le serveur utilise channel
       /*srv.channel.hook.
			.on(ON_START, &on_channel_start, NULL)
			.on(ON_UPDATE, &on_channel_update, NULL)
			.on(ON_SHUTDOWN, &on_channel_shutdown, NULL)
			.on(ON_CONNECT, &on_channel_connect, NULL)
			.on(ON_DISCONNECT, &on_channel_disconnect, NULL)
			.on(ON_ERROR, &on_channel_error, NULL)
			.on(ON_TIMEOUT, &on_channel_timeout, NULL)
			.on(ON_RECV, &on_channel_recv, NULL)
			.on(ON_SEND, &on_channel_send, NULL)
			.on("KICK", &handle_kick, NULL)
			.on("INVITE", &handle_invite, NULL)
			.on("TOPIC", &handle_topic, NULL)
			.on("MODE", &handle_mode, NULL);*/

		srv.channel(CHANNEL_NAME, CHANNEL_DEFAULT)
			.password("")
			.op(CHANNEL_OPERATOR)
			.size(CHANNEL_SIZE)
			.invite_only(CHANNEL_INV_ONLY)
			.restricted_topic(CHANNEL_REST_TOPIC)
			.topic(CHANNEL_TOPIC);

        // ================================
        // Lancement du serveur
        // ================================
        return srv.run();
    }
    catch (std::exception &e) {
        return (std::cerr << "Error: " << e.what() << std::endl, 1);
    }

    return 0;
}
