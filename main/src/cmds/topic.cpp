#include "Irc.hpp"

bool isSplitTopic(std::string cmd, std::string &channelName, std::string &topic, Client *client)
{
	std::istringstream ss(cmd);
	std::vector<std::string> stringContainer;
	std::string token;
	while (ss >> token)
		stringContainer.push_back(token);
	if (stringContainer.size() < 2)
		return (client->send(irc_err_needmoreparams(client->getNick(), cmd)), false);
	channelName = stringContainer[1];
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)), false);
	if (stringContainer.size() >= 3)
	{
		for (int i = 2; i < stringContainer.size(); ++i)
		{
			if (i != 2)
				topic += ' ';
			topic += stringContainer[i];
		}
	}
	return true;
}

void changeTopic(Channel *channel, Client *client, std::string topic)
{
	if (!channel->getRestrictedTopic())
		channel->setTopic(topic);
	else if (channel->getOperator(client->getFd()))
		channel->setTopic(topic);
	else
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channel->getName())));
}

void topic_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;
	std::string channelName;
	Channel *channel = NULL;
	std::string topic;
	std::string cmd = event->message->to<std::string>();
	if (!isSplitTopic(cmd, channelName, topic, client))
		return;
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (topic.empty())
	{
		std::string res = "No topic is set\n";
		if (!channel->getTopic().empty())
			res = channel->getTopic();
		return (client->send(irc_rpl_topic(client->getNick(), channelName, res)));
	}
	changeTopic(channel, client, topic);
}
