#include "Main.hpp"

bool isSplitkick(std::string cmd, std::string &channelName, std::string &nick, int fd)
{
	std::istringstream ss(cmd);
	std::vector<std::string> stringContainer;
	std::string token;
	while (ss >> token)
		stringContainer.push_back(token);
	if (stringContainer.size() < 3)
		return (write(fd, "ERR_NEEDMOREPARAMS (461)\n", strlen("ERR_NEEDMOREPARAMS (461)\n")), false);
	channelName = stringContainer[1];
	if (channelName[0] != '#')
		return (write(fd, "ERR_NOSUCHCHANNEL (403)\n", strlen("ERR_NOSUCHCHANNEL (403)\n")), false);
	if (stringContainer.size() >= 3)
		nick = channelName[2];
	return true;
}

void Server::kick(std::string cmd, int fd)
{
	std::cout << "CMD = " << cmd << std::endl;
	Client *op = NULL, *kickedClient = NULL;
	Channel *channel;
	std::string channelName;
	std::string nick;
	// Client kicked
	if(!isSplitkick(cmd, channelName, nick, fd))
		return;
	std::cout << "Nick = " << nick << " ChannelName " << channelName << std::endl;
	if(!(channel = getOneChannel(channelName)))
	{
		write(fd, "ERR_NOSUCHCHANNEL (403)\n", strlen("ERR_NOSUCHCHANNEL (403)\n"));
		return;
	}
	if(!(op = channel->getClient(fd)))
	{
		write(fd, "ERR_NOTONCHANNEL (442)\n", strlen("ERR_NOTONCHANNEL (442)\n"));
		return;
	}
	if(!(op = channel->getOperator(fd)))
	{
		write(fd, "ERR_CHANOPRIVSNEEDED (482)\n", strlen("ERR_CHANOPRIVSNEEDED (482)\n"));
		return;
	}
	if(!(kickedClient = getOneClient(nick)))
	{
		write(fd, "ERR_NOSUCHNICK (401)\n", strlen("ERR_NOSUCHNICK (401)\n"));
		return;
	}
	if(!(kickedClient = channel->getClient(nick)))
	{
		write(fd, "ERR_USERNOTINCHANNEL (401)\n", strlen("ERR_USERNOTINCHANNEL (401)\n"));
		return;
	}
	channel->removeClient(kickedClient->getFd());
	channel->broadcastToAll(nick + " kicked from" + "channelName");
	if(channel->getSize() < 1)
		deleteChannel(channelName);
	// verify that the user who write the command is admin
	// verify that the user who is kicked is in the channel

	// broadcast to all user of the channel exept the user kicked
}
