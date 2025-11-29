#include "Irc.hpp"

bool isSplitkick(std::string cmd, std::string &channelName, std::string &nick, Client *client)
{
	std::istringstream ss(cmd);
	std::vector<std::string> stringContainer;
	std::string token;
	while (ss >> token)
		stringContainer.push_back(token);
	if (stringContainer.size() < 3)
		return (client->send(irc_err_needmoreparams(client->getNick(), cmd)), false);
	channelName = stringContainer[1];
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)), false);
	if (stringContainer.size() >= 3)
		nick = channelName[2];
	return true;
}

void kick_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;

	Client *kickedClient = NULL;
	Channel *channel;
	std::string channelName;
	std::string nick;
	std::string cmd = event->message->to<std::string>();
	if (!isSplitkick(cmd, channelName, nick, client))
		return;
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (!(channel->getClient(client->getFd())))
		return (client->send(irc_err_notonchannel(client->getNick(), channelName)));
	if (!(channel->getOperator(client->getFd())))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channel->getName())));
	if (!(kickedClient = server->getOneClient(nick)))
		return (client->send(irc_err_nosuchnick(client->getNick(), nick)));
	if (!(channel->getClient(kickedClient->getFd())))
		return (client->send(irc_err_notonchannel(kickedClient->getNick(), channelName)));
	channel->removeClient(kickedClient->getFd());
	channel->broadcastToAll(nick + " kicked from" + "channelName");
	if (channel->getSize() < 1)
		server->deleteChannel(channelName);
}
