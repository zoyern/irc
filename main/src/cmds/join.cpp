#include "Irc.hpp"

bool isSplitJoin(std::string cmd, std::string &channelName, std::string &password, Client *client)
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
		password = stringContainer[2];
	return true;
}

void createChannel(std::string channelName, Client *client, Server *Server)
{
	if (channelName[0] != '#') // ERR_BADCHANMASK (476)
		return (client->send("Bad Channel Mask"));
	// :irc.example.com 476 user42 test :Bad Channel Mask
	Channel channel(channelName);
	channel.addClient(*client);
	channel.addOperator(*client);
	Server->addChannel(channel);
	client->send("Created channel " + channelName);
}

bool isError(Channel *channel, Client *client, std::string password, std::string cmd)
{
	if (channel->getClient(client->getFd()))
		return false;
	if (channel->getPassword() != password && !channel->getPassword().empty())
		return (client->send(irc_err_passwdmismatch(client->getNick(), cmd)), false);
	if (channel->getInviteOnly() && !channel->getInvitedClient(client->getFd()))
		return (client->send(irc_err_inviteonlychan(client->getNick(), channel->getName())), false);
	return true;
	if (channel->getLimit() > channel->getClients().size() + 1)
		return (client->send(irc_err_channelisfull(client->getNick(), channel->getName())), false);
	// if(user limit)
	// 	 ERR_TOOMANYCHANNELS (405)
}

void addClientTochannel(Channel *channel, Client *client)
{
	channel->addClient(*client);
	if (channel->getInvitedClient(client->getFd()))
		channel->removeInvitedClient(client->getFd());
	channel->broadcastToAll(client->getFd(), "new user joinded channel\n");
}

void join_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;
	Channel *channel;
	if (!client)
		return;
	std::string channelName;
	std::string password;
	std::string cmd = event->message->to<std::string>();
	if (!isSplitJoin(cmd, channelName, password, client))
		return;
	if (!(channel = server->getOneChannel(channelName)))
		return createChannel(channelName, client, server);
	if (!isError(channel, client, password, cmd))
		return;
	return addClientTochannel(channel, client);
}
