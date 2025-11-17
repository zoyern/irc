#include <irc.hpp>

Channel::Channel(void)
: _name("NULL")
, _password("")
, _topic("topic for :" + _name)
, _operator(-1), _size(-1)
, _inviteOnly(false)
, _restrictedTopic(false)
{return;}


Channel::Channel(std::string name, Client &client)
: _name("NULL")
, _password("")
, _topic("topic for :" + _name)
, _operator(-1), _size(-1)
, _inviteOnly(false)
, _restrictedTopic(false)
{
	_operators.push_back(client);
}
Channel::Channel(std::string name)
: _name(name)
, _password("")
, _topic("topic for :" + name)
, _operator(-1), _size(-1)
, _inviteOnly(false)
, _restrictedTopic(false)
{return;}
Channel::~Channel() {}

void Channel::setName(std::string name){_name = name;}
void Channel::setPassword(std::string password){_password = password;}
void Channel::setTopic(std::string topic){_topic = topic;}
void Channel::setSize(int size){_size = size;}
void Channel::setInviteOnly(bool invOnly){_inviteOnly = invOnly;}
void Channel::SetRestrictedTopic(bool restTopic){_restrictedTopic = restTopic;}


std::string Channel::getName(void) const{return _name;}
std::string Channel::getPassword(void) const{return _password;}
std::string Channel::getTopic(void) const{return _topic;}
int Channel::getSize(void) const{return _size;}
bool Channel::getInviteOnly(void) const {return _inviteOnly;}
bool Channel::getRestrictedTopic(void) const {return _restrictedTopic;}
Client *Channel::getClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();
	while(it != ite)
	{
		// find client by fd
	}
	return NULL;
}
Client *Channel::getOperator(int fd)
{
	std::vector<Client>::iterator it = _operators.begin();
	std::vector<Client>::iterator ite = _operators.end();
	while(it != ite)
	{
		// find operator by fd
	}
	return NULL;
}
Client *Channel::getInvitedClient(int fd)
{
	std::vector<Client>::iterator it = _operators.begin();
	std::vector<Client>::iterator ite = _operators.end();
	while(it != ite)
	{
		// find invitation by fd
	}
	return NULL;
}

void Channel::addClient(Client &newClient)
{
	// verify if client exist, if not, then add;
}
void Channel::addOperator(Client &newOperator)
{
	// verify if operator exist, if not, then add;
}
void Channel::addInvitation(Client &newOperator)
{
	// verify if operator exist, if not, then add;
}

void Channel::removeClient(int fd)
{
	// if client exist then remove
}
void Channel::removeOperator(int fd)
{
	// if admin exist then remove
}

void Channel::removeInvitedClient(int fd)
{
	// if admin exist then remove
}

void Channel::broadcastToAll(int clientFd)
{
	char buffer[1024];
	while(true)
	{
		int bytesRead = read(clientFd, buffer, sizeof(buffer));
		if(bytesRead <= 0)
			break;
		// for everry clientfd, exept current client, brodcast the message
		// write(otherClientfd, buffer, bytesRead);
	}
}

void Channel::broadcastToAll(int clientFd, std::string msg)
{
	char buffer[1024];
	while(true)
	{
		int bytesRead = read(clientFd, buffer, sizeof(buffer));
		if(bytesRead <= 0)
			break;
		// for everry clientfd, exept current client, brodcast the message
		// write(otherClientfd, buffer, bytesRead);
	}
}
