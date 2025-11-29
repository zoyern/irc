#include "Irc.hpp"

Channel::Channel(void)
	: _name(""), _password(""), _topic(""), _size(-1), _inviteOnly(false), _restrictedTopic(false)
{
	return;
}

Channel::Channel(std::string name, Client &client)
	: _name(name), _password(""), _topic(""), _size(-1), _inviteOnly(false), _restrictedTopic(false)
{
	_operators.push_back(client);
}
Channel::Channel(std::string name)
	: _name(name), _password(""), _topic(""), _size(-1), _inviteOnly(false), _restrictedTopic(false)
{
	return;
}
Channel::~Channel() {}

void Channel::setName(std::string name) { _name = name; }
void Channel::setPassword(std::string password) { _password = password; }
void Channel::setTopic(std::string topic) { _topic = topic; }
void Channel::setLimit(int limit) { _limit = limit; }
void Channel::setSize(int size) { _limit = size; }
void Channel::setInviteOnly(bool invOnly) { _inviteOnly = invOnly; }
void Channel::SetRestrictedTopic(bool restTopic) { _restrictedTopic = restTopic; }

std::string Channel::getName(void) const { return _name; }

std::string Channel::getPassword(void) const { return _password; }

std::string Channel::getTopic(void) const { return _topic; }

int Channel::getLimit(void) const { return _limit; }

int Channel::getSize(void) const { return _limit; }

bool Channel::getInviteOnly(void) const { return _inviteOnly; }

bool Channel::getRestrictedTopic(void) const { return _restrictedTopic; }

Client *Channel::getClient(std::string nick)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();
	while (it != ite)
	{
		if (it->getNick() == nick)
			return &(*it);
		it++;
	}
	return NULL;
}
Client *Channel::getClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();
	while (it != ite)
	{
		if (it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}

std::vector<Client> Channel::getOperators(void) const
{
	return _operators;
}

Client *Channel::getOperator(int fd)
{
	std::vector<Client>::iterator it = _operators.begin();
	std::vector<Client>::iterator ite = _operators.end();
	while (it != ite)
	{
		if (it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}
Client *Channel::getInvitedClient(int fd)
{
	std::vector<Client>::iterator it = _operators.begin();
	std::vector<Client>::iterator ite = _operators.end();
	while (it != ite)
	{
		if (it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}

std::vector<Client> Channel::getClients(void) const
{
	return _clients;
}

void Channel::addClient(Client &newClient)
{
	if (!getClient(newClient.getFd()))
		_clients.push_back(newClient);
}
void Channel::addOperator(Client &newOperator)
{
	if (!getOperator(newOperator.getFd()) && getClient(newOperator.getFd()))
		_operators.push_back(newOperator);
}
void Channel::addInvitation(Client &newinvited)
{
	if (!getClient(newinvited.getFd()) && !getInvitedClient(newinvited.getFd()))
		_inviteds.push_back(newinvited);
}

void Channel::removeClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++)
	{
		if (it->getFd() == fd)
			_clients.erase(it);
	}
	removeOperator(fd);
	removeInvitedClient(fd);
}
void Channel::removeOperator(int fd)
{
	std::vector<Client>::iterator it = _operators.begin();
	for (; it != _operators.end(); it++)
	{
		if (it->getFd() == fd)
			_operators.erase(it);
	}
}

void Channel::removeInvitedClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++)
	{
		if (it->getFd() == fd)
			_clients.erase(it);
	}
}

void Channel::broadcastToOne(int fd, std::string msg)
{
	Client *client = getClient(fd);
	if (client)
		client->send(msg);
}

void Channel::broadcastToAll(std::string msg)
{
	broadcastToAll(-1, msg);
}

void Channel::broadcastToAll(int clientFd, std::string msg)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();

	while (it != ite)
	{
		if (it->getFd() == clientFd)
			continue;
		it->send(msg);
		it++;
	}
}
