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
        return (Console::instance().error("Usage") << "./ircserv <protocol> [port]", 1);

    try {
        SkllServer srv(argv[1], argv[2]);

        // ================================
        // Configuration générale
        // ================================
        srv.network
			.name("ircserv")               // Nom serveur
			.address("0.0.0.0")            // Écoute sur toutes les interfaces
			.connexion_msg("Welcome!")     // Message de connexion
			.max_clients(100)              // Limite clients
			.reserved_fds(10)              // RéSkllServer FDs
			.timeout(30)                   // Timeout en secondes
			.queue(128);                   // Queue backlog

        // ================================
        // Hooks
        // ================================
        srv.hooks
			.on(ON_START, &start, NULL)
			.on(ON_UPDATE, &update, NULL)
			.on(ON_SHUTDOWN, &shutdown, NULL)
			.on(ON_CONNECT, &on_connect, NULL)
			.on(ON_DISCONNECT, &on_disconnect, NULL)
			.on(ON_ERROR, &on_error, NULL)
			.on(ON_TIMEOUT, &on_timeout, NULL)
			.on(ON_RECV, &on_recv, NULL)
			.on(ON_SEND, &on_send, NULL);

        // ================================
        // Protocole IRC
        // ================================
        srv.protocol("irc", "\r\n", false, TCP)
           .buffer_size(2048)
           .on("NICK", handle_nick, NULL)
           .on("JOIN", handle_join, NULL)
           .on("PRIVMSG", handle_privmsg, NULL)

		   // commandes server
		   // commandes channels

        // ================================
        // Channels
        // ================================ // re réflechir a comment le serveur utilise channel
        srv.channel.add(CHANNEL_NAME, CHANNEL_DEFAULT)
           .password("")
           .op(CHANNEL_OPERATOR)
           .size(CHANNEL_SIZE)
           .invite_only(CHANNEL_INV_ONLY)
           .restricted_topic(CHANNEL_REST_TOPIC)
           .topic(CHANNEL_TOPIC)

			.on("NICK", handle_nick, NULL)
           .on("JOIN", handle_join, NULL)
           .on("PRIVMSG", handle_privmsg, NULL)
		
		   .hook(ON_START, &start, NULL)
           .hook(ON_UPDATE, &update, NULL)&
           .hook(ON_SHUTDOWN, &shutdown, NULL)
           .hook(ON_CONNECT, &on_connect, NULL)
           .hook(ON_DISCONNECT, &on_disconnect, NULL)
           .hook(ON_ERROR, &on_error, NULL)
           .hook(ON_TIMEOUT, &on_timeout, NULL)
           .hook(ON_RECV, &on_recv, NULL)
           .hook(ON_SEND, &on_send, NULL);


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
