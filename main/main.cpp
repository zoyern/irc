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

#include <Sockell/SkllServer.hpp>
#include <Sockell/SkllClient.hpp>
#include <cstdlib>
#include <iostream>
#include <map>

#define SKLL_MAX_CLIENTS  1000
#define SKLL_RESERVED_FD  10
#define SKLL_TIMEOUT      100
#define SKLL_QUEUE        128

std::map<int, SkllClient> clients;

void on_start(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] START" << std::endl;
}

void on_update(SkllHookData* d) {
    (void)d;
}

void on_shutdown(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] SHUTDOWN" << std::endl;
}

void on_connect(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] CONNECT" << std::endl;
}

void on_disconnect(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] DISCONNECT" << std::endl;
}

void on_error(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] ERROR" << std::endl;
}

void on_timeout(SkllHookData* d) {
    (void)d;
}

void on_recv(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] RECV" << std::endl;
}

void on_send(SkllHookData* d) {
    (void)d;
    std::cout << "[HOOK] SEND" << std::endl;
}

int main(int argc, char const **argv)
{
    if (argc != 3) {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    try {
        SkllServer server(SKLL_MAX_CLIENTS, SKLL_RESERVED_FD);
        
        SkllNetwork& network = server.network("network", SKLL_TIMEOUT, SKLL_QUEUE)
            .on(ON_START, &on_start, NULL)
            .on(ON_UPDATE, &on_update, NULL)
            .on(ON_SHUTDOWN, &on_shutdown, NULL)
            .on(ON_CONNECT, &on_connect, NULL)
            .on(ON_DISCONNECT, &on_disconnect, NULL)
            .on(ON_ERROR, &on_error, NULL)
            .on(ON_TIMEOUT, &on_timeout, NULL)
            .on(ON_RECV, &on_recv, NULL)
            .on(ON_SEND, &on_send, NULL);
        
        network.listen("irc", "0.0.0.0", std::atoi(argv[1]), SKLL_TCP | SKLL_IPV4)
            .set_timeout(60, 10, 3)
            .set_crlf("\r\n")
            .set_buffer_size(512)
            .set_chunk_size(16);

        server.channel("general", true)
            .set_password("")
            .set_topic("Welcome to Sockell IRC!");

        std::cout << "\n===== Server Ready =====" << std::endl;
        std::cout << server.print_networks() << std::endl;
        std::cout << "==========================" << std::endl << std::endl;
        
        return server.run();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}