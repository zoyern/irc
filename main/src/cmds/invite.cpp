#include "Irc.hpp"

bool isSplitInvite(std::string cmd, std::string &channelName, std::string &nick, Client *client)
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

void invite_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;

	Client *invitedClient = NULL;
	Channel *channel;
	std::string channelName;
	std::string nick;
	std::string cmd = event->message->to<std::string>();
	if (!isSplitInvite(cmd, channelName, nick, client))
		return;
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (!(channel->getClient(client->getFd())))
		return (client->send(irc_err_notonchannel(client->getNick(), channelName)));
	if (channel->getInviteOnly() && !(channel->getOperator(client->getFd())))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channel->getName())));
	if (!(invitedClient = server->getOneClient(nick)))
		return (client->send(irc_err_nosuchnick(client->getNick(), nick)));
	if (channel->getClient(invitedClient->getFd()))
		return (invitedClient->send(irc_err_useronchannel(invitedClient->getNick(), channelName)));
	if (!channel->getInvitedClient(invitedClient->getFd()))
		channel->addInvitation(*invitedClient);
	invitedClient->send(irc_rpl_invite(client->getNick(), invitedClient->getNick(), channelName));
	// cmd: INVITE nickName Channel
	// verify if user is a memeber of channel
	// verify if target exist
	// verify if target user is not already on channel, or
	// hes been already invited

	// sent message to invited user only
}
