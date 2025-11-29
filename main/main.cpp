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

#include <IRC.hpp>

bool checkPort(std::string strPort)
{
	if (strPort.find_first_not_of("0123456789") != std::string::npos)
		return (std::cerr << RED << "Error: port must only contain numbers" << RESET << std::endl, false);
	int port = std::atoi(strPort.c_str());
	if (port < 1024 || port > 65535)
		return (std::cerr << RED << "Error: Port must be between 1024-65535" << RESET << std::endl, false);
	return true;
}
std::vector<std::string> splitCmdLine(std::string str)
{
	std::vector<std::string> cmds;
	std::istringstream stm(str);
	std::string line;
	while (std::getline(stm, line))
	{
		size_t pos = line.find_first_of("\r\n");
		if (pos != std::string::npos)
			line = line.substr(0, pos);
		cmds.push_back(line);
	}
	return cmds;
}
std::vector<std::string> splitCmd(std::string input)
{
	std::vector<std::string> out;
	std::string commandLine = input;

	std::istringstream ss(commandLine);
	std::string token;

	if (!(ss >> token))
		return out;

	for (int i = 0; token[i]; ++i)
	{
		token[i] = std::tolower(static_cast<unsigned char>(token[i]));
	}

	out.push_back(token);

	while (ss >> token)
		out.push_back(token);

	return out;
}

int main(int ac, char **av)
{
	if (ac != 3)
		return (std::cout << RED << "Usage: ./ircserv <port> <password>" << RESET << std::endl, EXIT_FAILURE);
	if (!checkPort(av[1]))
		return 1;
	IRCServer server(std::atoi(av[1]), av[2]);
	try
	{
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	std::cout << "Server closed !" << std::endl;
	return 0;
}
/*
int main(int ac, char **av)
{
	if (ac != 3)
		return (std::cout << RED << "Usage: ./ircserv <port> <password>" << RESET << std::endl, EXIT_FAILURE);
	if (!checkPort(av[1]))
		return 1;
	Server server(std::atoi(av[1]), av[2]);
	try
	{
		// handle Signals 
		server.initServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	std::cout << "Server closed !" << std::endl;
	return 0;
}*/ 