/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcClient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:36:20 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:36:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Irc.hpp"

class Client
{
public:
	Client(void);
	Client(int fd);
	Client(std::string n, std::string u);
	Client(Client const &client);
	Client &operator=(Client const &client);
	void setUser(std::string user);
	void setFd(int fd);
	void setNick(std::string nick);
	void setIsPassCorrect(bool b);
	void setAttempts(void);
	std::string getUser(void) const;
	std::string getNick(void) const;
	int getAttempts(void) const;
	int getFd(void) const;
	bool getisPassCorrect(void) const;
	bool isAuth();
	void send(std::string msg);
	~Client();

private:
	std::string _nick;
	std::string _user;
	int _fd;
	bool _isPassCorrect;
	int _attempts;
};
