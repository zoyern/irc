#include "Irc.hpp"

void mode_cmd_i(SkllEvent *event, void *user_data) // MODE_cmd #salon +i
{
	Server *server = (Server *)user_data;
	Client *client = (Client *)event->client->userdata;
	if (!client)
		return;
	Channel *channel = NULL;
	std::string channelName = "#test";
	bool invitOnly = true;
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (!channel->getClient(client->getFd()))
		return (client->send(irc_err_notonchannel(client->getNick(), channelName)));
	if (channel->getOperator(client->getFd()))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channelName)));
	channel->setInviteOnly(invitOnly);
}

void mode_cmd_t(SkllEvent *event, void *user_data) // MODE_cmd #salon +t
{
	Server *server = (Server *)user_data;
	Client *client = (Client *)event->client->userdata;
	Channel *channel = NULL;
	std::string channelName = "#test";
	if (!client)
		return;
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channel->getOperator(client->getFd()))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channelName)));
	channel->SetRestrictedTopic(true);
}
void mode_cmd_k(SkllEvent *event, void *user_data) // MODE_cmd #salon +k secret
{
	Server *server = (Server *)user_data;
	Client *client = (Client *)event->client->userdata;
	if (!client)
		return;
	Channel *channel = NULL;
	std::string channelName = "#test";
	std::string password = "password";
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channel->getOperator(client->getFd()))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channelName)));
	channel->setPassword(password);
}
void mode_cmd_o(SkllEvent *event, void *user_data) // MODE_cmd #salon +o Alice
{
	Server *server = (Server *)user_data;
	Client *client = (Client *)event->client->userdata;
	if (!client)
		return;
	Channel *channel = NULL;
	std::string nick = "nick";
	Client *otherClient = NULL;
	std::string channelName = "#test";
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (!channel->getClient(client->getFd()))
		return (client->send(irc_err_notonchannel(client->getNick(), channelName)));
	if (!(otherClient = server->getOneClient(nick)))
		return (client->send(irc_err_nosuchnick(otherClient->getNick(), nick)));
	if (!channel->getClient(otherClient->getFd()))
		return (client->send(irc_err_notonchannel(otherClient->getNick(), channelName)));
	if (channel->getOperator(client->getFd()))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channelName)));

	if (!channel->getOperator(otherClient->getFd()))
		channel->addOperator(*otherClient);
}
void mode_cmd_l(SkllEvent *event, void *user_data) // MODE_cmd #salon +l 50
{
	Server *server = (Server *)user_data;
	Client *client = (Client *)event->client->userdata;
	if (!client)
		return;
	Channel *channel = NULL;
	std::string channelName = "#test";
	int limit = 50;
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (channelName[0] != '#')
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (!channel->getClient(client->getFd()))
		return (client->send(irc_err_notonchannel(client->getNick(), channelName)));
	if (channel->getOperator(client->getFd()))
		return (client->send(irc_err_chanoprivsneeded(client->getNick(), channelName)));
	if (limit < 1)
		channel->setLimit(-1);
	else
		channel->setLimit(limit);
}
void mode_cmd(int fd, std::string mode)
{

	// verify if user is admin

	// verify if mode is [i, t, k, o, l]
	// if +, add mode
	// if -, remove mode

	// brodcast to all client off the channel
}
