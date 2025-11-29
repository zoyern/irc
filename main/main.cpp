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


#include <IRCServer.hpp>
#include <iostream>
#include <cstdlib>

static void usage(const char *prog) {
	std::cout << "Usage: " << prog << " <port> <password>" << std::endl;
	std::cout << std::endl;
	std::cout << "Arguments:" << std::endl;
	std::cout << "  port      Port number (1024-65535)" << std::endl;
	std::cout << "  password  Server password (required for all clients)" << std::endl;
	std::cout << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "  " << prog << " 6667 secret123" << std::endl;
	std::cout << "  " << prog << " 6697 mypassword" << std::endl;
	std::cout << std::endl;
	std::cout << "Testing:" << std::endl;
	std::cout << "  nc -C localhost 6667          # Manual test with netcat" << std::endl;
	std::cout << "  python3 test_all.py 6667 secret123  # Automated tests" << std::endl;
}

int main(int argc, char ** argv) {
	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	/* Validate port */
	int port = std::atoi(argv[1]);
	if (port < 1024 || port > 65535) {
		std::cerr << "Error: Port must be between 1024 and 65535" << std::endl;
		return 1;
	}

	/* Validate password */
	std::string password = argv[2];
	if (password.empty()) {
		std::cerr << "Error: Password cannot be empty" << std::endl;
		return 1;
	}

	/* Start server */
	try {
		IRCServer server(port, password);
		server.start();
	} catch (const std::exception &e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
