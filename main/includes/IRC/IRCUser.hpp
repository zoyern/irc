/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCUser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 19:12:18 by marvin            #+#    #+#             */
/*   Updated: 2025/11/28 19:12:18 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <IRC.hpp>

class IRCUser
{
public:
	IRCUser(void);
	IRCUser(int fd);
	IRCUser(std::string p, std::string n, std::string u);
	IRCUser(IRCUser const &IRCUser);
	IRCUser &operator=(IRCUser const &IRCUser);
	void setUser(std::string user);
	void setFd(int fd);
	void setNick(std::string nick);
	void setPass(std::string pass);
	void setBuffer(std::string buf);
	void clearBuffer(void);
	std::string getUser(void) const;
	std::string getNick(void) const;
	std::string getPass(void) const;
	int getFd(void) const;
	std::string getBuffer(void) const;
	bool isAuth();
	void send(std::string msg);
	~IRCUser();

private:
	std::string _pass;
	std::string _nick;
	std::string _user;
	std::string _buffer;
	int _fd;
};