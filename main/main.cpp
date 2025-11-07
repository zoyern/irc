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

int main(int argc, char const **argv)
{
	if (argc < 2 || argc > 3) return (std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl, 1);
	try {
		Server srv(argv[1], argv[2]).name("irc")
			.adress("0.0.0.0")
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

int validate_port(const char* str) {
    char* end;
    long port = strtol(str, &end, 10);
    return (*end || port < 1 || port > 65535) ? -1 : (int)port;
}

int main(int argc, char** argv) {
    if (argc != 3)
        return (std::cerr << "Usage: " << argv[0] << " <port> <password>\n", 1);
    
    int port = validate_port(argv[1]);
    if (port < 0)
        return (std::cerr << "Invalid port\n", 1);
    
    // Socket
    int srv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv < 0)
        return (std::cerr << "socket() failed\n", 1);
    
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    fcntl(srv, F_SETFL, O_NONBLOCK);
    
    // Bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        return (close(srv), std::cerr << "bind() failed\n", 1);
    
    // Listen
    if (listen(srv, SOMAXCONN) < 0)
        return (close(srv), std::cerr << "listen() failed\n", 1);
    
    std::cout << "Server listening on port " << port << std::endl;
    
    // Epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
        return (close(srv), std::cerr << "epoll_create1() failed\n", 1);
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = srv;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv, &ev);
    
    // Loop
    struct epoll_event events[128];
    while (true) {
        int n = epoll_wait(epoll_fd, events, 128, -1);
        
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == srv) {
                // New client
                int cli = accept(srv, NULL, NULL);
                if (cli < 0) continue;
                
                std::cout << "New client (fd=" << cli << ")\n";
                
                fcntl(cli, F_SETFL, O_NONBLOCK);
                ev.events = EPOLLIN;
                ev.data.fd = cli;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli, &ev);
                
            } else {
                // Client data
                int cli = events[i].data.fd;
                char buf[512];
                int bytes = recv(cli, buf, 512, 0);
                
                if (bytes <= 0) {
                    std::cout << "Client disconnected (fd=" << cli << ")\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cli, NULL);
                    close(cli);
                } else {
                    buf[bytes] = '\0';
                    std::cout << "[" << cli << "] " << buf;
                    
                    // Echo back
                    send(cli, buf, bytes, 0);
                }
            }
        }
    }
    
    close(srv);
    close(epoll_fd);
    return 0;
}

