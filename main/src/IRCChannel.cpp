
#include <IRC.hpp>

IRCChannel::IRCChannel(void)
	: _name(""), _password(""), _topic(""), _size(-1), _inviteOnly(false), _restrictedTopic(false)
{
	return;
}

IRCChannel::IRCChannel(std::string name, IRCUser &client)
	: _name(name), _password(""), _topic(""), _size(-1), _inviteOnly(false), _restrictedTopic(false)
{
	_operators.push_back(client);
}
IRCChannel::IRCChannel(std::string name)
	: _name(name), _password(""), _topic(""), _size(-1), _inviteOnly(false), _restrictedTopic(false)
{
	return;
}
IRCChannel::~IRCChannel() {}

void IRCChannel::setName(std::string name) { _name = name; }
void IRCChannel::setPassword(std::string password) { _password = password; }
void IRCChannel::setTopic(std::string topic) { _topic = topic; }
void IRCChannel::setSize(int size) { _size = size; }
void IRCChannel::setInviteOnly(bool invOnly) { _inviteOnly = invOnly; }
void IRCChannel::SetRestrictedTopic(bool restTopic) { _restrictedTopic = restTopic; }

std::string IRCChannel::getName(void) const { return _name; }
std::string IRCChannel::getPassword(void) const { return _password; }
std::string IRCChannel::getTopic(void) const { return _topic; }
int IRCChannel::getSize(void) const { return _size; }
bool IRCChannel::getInviteOnly(void) const { return _inviteOnly; }
bool IRCChannel::getRestrictedTopic(void) const { return _restrictedTopic; }
IRCUser *IRCChannel::getClient(std::string nick)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	std::vector<IRCUser>::iterator ite = _clients.end();
	while (it != ite)
	{
		if(it->getNick() == nick)
			return &(*it);
		it++;
	}
	return NULL;
}
IRCUser *IRCChannel::getClient(int fd)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	std::vector<IRCUser>::iterator ite = _clients.end();
	while (it != ite)
	{
		if(it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}
IRCUser *IRCChannel::getOperator(int fd)
{
	std::vector<IRCUser>::iterator it = _operators.begin();
	std::vector<IRCUser>::iterator ite = _operators.end();
	while (it != ite)
	{
		if(it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}
IRCUser *IRCChannel::getInvitedClient(int fd)
{
	std::vector<IRCUser>::iterator it = _operators.begin();
	std::vector<IRCUser>::iterator ite = _operators.end();
	while (it != ite)
	{
		if(it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}

void IRCChannel::addClient(IRCUser &newClient)
{
	if (!getClient(newClient.getFd()))
		_clients.push_back(newClient);
}
void IRCChannel::addOperator(IRCUser &newOperator)
{
	if (!getOperator(newOperator.getFd()) && getClient(newOperator.getFd()))
		_operators.push_back(newOperator);
}
void IRCChannel::addInvitation(IRCUser &newinvited)
{
	if (!getClient(newinvited.getFd()) && !getInvitedClient(newinvited.getFd()))
		_inviteds.push_back(newinvited);
}

void IRCChannel::removeClient(int fd)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
	{
		if(it->getFd() == fd)
			_clients.erase(it);
	}
	removeOperator(fd);
}
void IRCChannel::removeOperator(int fd)
{
	std::vector<IRCUser>::iterator it = _operators.begin();
	for(; it != _operators.end(); it++)
	{
		if(it->getFd() == fd)
			_operators.erase(it);
	}
}

void IRCChannel::removeInvitedClient(int fd)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	for(; it != _clients.end(); it++)
	{
		if(it->getFd() == fd)
			_clients.erase(it);
	}
}

void IRCChannel::broadcastToOne(int fd, std::string msg)
{
	IRCUser *client = getClient(fd);
	if (client)
		write(client->getFd(), msg.c_str(), msg.length());
}

void IRCChannel::broadcastToAll(std::string msg)
{
	broadcastToAll(-1, msg);
}

void IRCChannel::broadcastToAll(int clientFd, std::string msg)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	std::vector<IRCUser>::iterator ite = _clients.end();

	while (it != ite)
	{
		if (it->getFd() == clientFd)
			continue;
		write(it->getFd(), msg.c_str(), msg.length());
		it++;
	}
}