#include "Irc.hpp"

bool isSplitTopic(std::string cmd, std::string &channelName, std::string &topic, int fd)
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
	{
		for(int i = 2; i < stringContainer.size(); ++i)
		{
			if(i != 2)
				topic += ' ';
			topic += stringContainer[i];
		}
	}
	return true;
}

void changeTopic(Channel *channel, Client *client, std::string topic)
{
	if(!channel->getRestrictedTopic())
		channel->setTopic(topic);
	else if(channel->getOperator(client->getFd()))
		channel->setTopic(topic);
	else
	{
		write(client->getFd(), "ERR_CHANOPRIVSNEEDED (403)\n", strlen("ERR_CHANOPRIVSNEEDED (403)\n"));
		return;
	}
}

void Server::topic(std::string cmd, int fd)
{
	std::string channelName;
	std::string topic;
	Channel *channel = NULL;
	Client *client = NULL;
	if (!(client = getOneClient(fd)))
	{
		write(fd, "ERR_NOTONCHANNEL\n", strlen("ERR_NOTONCHANNEL\n"));
		return;
	}
	if (!isSplitTopic(cmd, channelName, topic, fd))
		return;
	if (!(channel = getOneChannel(channelName)))
	{
		write(fd, "ERR_NOSUCHCHANNEL (403)\n", strlen("ERR_NOSUCHCHANNEL (403)\n"));
		return;
	}
	if (topic.empty())
	{
		std::string res = "No topic is set\n";
		if (!channel->getTopic().empty())
			res = "topic is " + channel->getTopic();
		write(fd, res.c_str(), strlen(res.c_str()));
	}
	changeTopic(channel, client, topic);
	// verify if channel exist
	// verify if user is in channel
	// verify topic restriction
	// then change topic name
	// broadcast to all user of the channel
}
