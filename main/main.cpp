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

void on_connect(void* lib, void* user) {
    if (!lib || !user) return;
    int* fd = static_cast<int*>(lib);
    IRCServer* irc = static_cast<IRCServer*>(user);
    irc->on_connect(*fd);
}

void on_recv(void* lib, void* user) {
    (void)lib;
    if (!user) return;
    IRCServer* irc = static_cast<IRCServer*>(user);
    irc->on_recv();
}

int main(int argc, char const **argv) {
    if (argc != 3) {
        std::cout << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    try {
        IRCServer irc(argv[2]);
        
        SkllProtocol	proto("irc", "0.0.0.0", std::atoi(argv[1]), SKLL_TCP | SKLL_IPV4);
		SkllNetwork		network("net", 100, 128);
		SkllServer		server(IRC_MAX_CLIENTS, 10);

		server.networks(network)
			.listen(proto)
			.on(ON_CONNECT, on_connect, &irc)
        	.on(ON_RECV, on_recv, &irc);
        
        return server.run();
    }
    catch (SkllException& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}