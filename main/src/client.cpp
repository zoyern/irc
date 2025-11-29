
#include "Irc.hpp"
Client::Client(void) : _nick(""), _user(""), _fd(-1), _isPassCorrect(false), _attempts(0) {}
Client::Client(int fd) : _nick(""), _user(""), _fd(fd), _isPassCorrect(false), _attempts(0) {}
Client::Client(std::string n, std::string u) : _nick(n), _user(u), _fd(-1), _isPassCorrect(false), _attempts(0) {}
Client::Client(const Client &client)
{
	*this = client;
}


Client &Client::operator=(const Client &client)
{
	if (this != &client)
	{
		this->_user = client.getUser();
		this->_nick = client.getNick();
		this->_fd = client.getFd();
	}
	return *this;
}
void Client::setUser(std::string user)
{
	_user = user;
}
void Client::setFd(int fd)
{
	_fd = fd;
}
void Client::setNick(std::string nick)
{
	_nick = nick;
}
void Client::setIsPassCorrect(bool b)
{
	_isPassCorrect = b;
}
void Client::setAttempts(void)
{
	_attempts++;
}

int Client::getAttempts(void) const
{
	return _attempts;
}

std::string Client::getUser(void) const
{
	return _user;
}
std::string Client::getNick(void) const
{
	return _nick;
}
bool Client::getisPassCorrect(void) const
{
	return _isPassCorrect;
}
int Client::getFd(void) const
{
	return _fd;
}
bool Client::isAuth()
{
	return (_fd != -1 && getisPassCorrect() && !_nick.empty() && !_user.empty());
}

void Client::send(std::string msg)
{
	write(getFd(), msg.c_str(), strlen(msg.c_str()));
}
Client::~Client()
{
	// if(_fd != -1)
	// {
	// 	close(_fd);
	// 	std::cout << _nick  << " has been deleted !" << std::endl;
	// }
}
