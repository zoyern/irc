#include "Irc.hpp"

void part_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;
	Channel *channel = NULL;
	std::string channelName = "#test";
	// if( params < 2)
	// 	return (client->send(irc_err_needmoreparams(client->getNick(), cmd)));
	if (!(channel = server->getOneChannel(channelName)))
		return (client->send(irc_err_nosuchchannel(client->getNick(), channelName)));
	if (!(channel->getClient(client->getFd())))
		return (client->send(irc_err_notonchannel(client->getNick(), channelName)));
	channel->removeClient(client->getFd());
	if (!(channel = server->getOneChannel(channelName)) && (channel->getOperators()).size() < 1)
		channel->addOperator(channel->getClients().front());
}
