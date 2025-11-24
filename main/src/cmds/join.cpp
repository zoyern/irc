#include "Main.hpp"

bool isError(Channel *channel, Client *client, std::string password)
{
	if (channel->getClient(client->getFd()))
		return false;
	if (channel->getPassword() != password)
		return (write(client->getFd(), "ERR_PASSWDMISMATCH 464\n", strlen("ERR_PASSWDMISMATCH 464\n")), false);
	if (channel->getInviteOnly() && !channel->getInvitedClient(client->getFd()))
		return (write(client->getFd(), "ERR_INVITEONLYCHAN 473\n", strlen("ERR_INVITEONLYCHAN 473\n")), false);
	return true;
	// if(channel limit)
	// 	return false; ERR_CHANNELISFULL (471)
	// if(user limit)
	// 	 ERR_TOOMANYCHANNELS (405)
}

bool isSplitJoin(std::string cmd, std::string &channelName, std::string &password, int fd)
{
	std::istringstream ss(cmd);
	std::vector<std::string> stringContainer;
	std::string token;
	while (ss >> token)
		stringContainer.push_back(token);
	if (stringContainer.size() < 2)
		return (write(fd, "ERR_NEEDMOREPARAMS (461)\n", strlen("ERR_NEEDMOREPARAMS (461)\n")), false);
	channelName = stringContainer[1];
	if (channelName[0] != '#')
		return (write(fd, "ERR_NOSUCHCHANNEL (403)\n", strlen("ERR_NOSUCHCHANNEL (403)\n")), false);
	if (stringContainer.size() >= 3)
		password = stringContainer[2];
	return true;
}

void createChannel(std::string channelName, Client *client, Server *Server)
{
	Channel channel(channelName);
	channel.addClient(*client);
	channel.addOperator(*client);
	Server->addChannel(channel);
	write(client->getFd(), "Channel created\n", strlen("Channel created\n"));
}

void addClientTochannel(Channel *channel, Client *client)
{
	channel->addClient(*client);
	if (channel->getInvitedClient(client->getFd()))
		channel->removeInvitedClient(client->getFd());
	channel->broadcastToAll(client->getFd(), "new user joinded channel\n");
}
void Server::join(std::string cmd, int fd)
{
	Client *client = getOneClient(fd);
	if (!client)
		return;
	std::string channelName;
	std::string password;
	if (!isSplitJoin(cmd, channelName, password, fd))
		return;
	Channel *channel = getOneChannel(channelName);
	if (!channel)
		return createChannel(channelName, client, this);
	if (!isError(channel, client, password))
		return;
	return addClientTochannel(channel, client);
	// channel->broadcastToAll("msg");
	// if channel is invite only, verify if user has been invited.
	// 	if user have been invited, then add user, and delete user
	// 	from invitedlist

	// if channel required a password, compare password
	// 	if passwords are the same, then add userw

	// brodcast to all client off the channel
}
