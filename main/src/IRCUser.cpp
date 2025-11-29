/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:15:01 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:15:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <IRC.hpp>

IRCUser::IRCUser(void) : _pass(""), _nick(""), _user(""), _fd(-1) {}
IRCUser::IRCUser(int fd) : _pass(""), _nick(""), _user(""), _fd(fd) {}
IRCUser::IRCUser(std::string p, std::string n, std::string u) : _pass(p), _nick(n), _user(u), _fd(-1) {}
IRCUser::IRCUser(const IRCUser &IRCUser)
{
	*this = IRCUser;
}

IRCUser &IRCUser::operator=(const IRCUser &IRCUser)
{
	if (this != &IRCUser)
	{
		this->_user = IRCUser.getUser();
		this->_nick = IRCUser.getNick();
		this->_pass = IRCUser.getPass();
		this->_fd = IRCUser.getFd();
	}
	return *this;
}
void IRCUser::setUser(std::string user)
{
	_user = user;
}
void IRCUser::setFd(int fd)
{
	_fd = fd;
}
void IRCUser::setNick(std::string nick)
{
	_nick = nick;
}
void IRCUser::setPass(std::string pass)
{
	_pass = pass;
}

void IRCUser::setBuffer(std::string str)
{
	_buffer += str;
}

void IRCUser::clearBuffer(void)
{
	_buffer.clear();
}

std::string IRCUser::getUser(void) const
{
	return _user;
}
std::string IRCUser::getNick(void) const
{
	return _nick;
}
std::string IRCUser::getPass(void) const
{
	return _pass;
}
int IRCUser::getFd(void) const
{
	return _fd;
}

std::string IRCUser::getBuffer(void) const
{
	return _buffer;
}
bool IRCUser::isAuth()
{
	return (_fd != -1 && !_pass.empty() && !_nick.empty() && !_user.empty());
}

void IRCUser::send(std::string msg)
{
	write(getFd(), msg.c_str(), strlen(msg.c_str()));
}

IRCUser::~IRCUser()
{
	// if(_fd != -1)
	// {
	// 	close(_fd);
	// 	std::cout << _nick  << " has been deleted !" << std::endl;
	// }
}