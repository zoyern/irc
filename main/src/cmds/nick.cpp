#include "Irc.hpp"

void nick_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;
	if (!client) return;
	if(!client->getisPassCorrect())
		return client->send(Errors::not_authenticated());
	std::string nick = event->message->to<std::string>();
	if(nick.empty())
		return client->send("Commande error: need more params\n"); //ERR_NEEDMOREPARAMS
	if(server->getOneClient(nick))
		return client->send("Error: " + nick + " is already in use");
	client->setNick(nick);
	if(client->isAuth())
		client->send("Welcome " + client->getNick() + "\n");
}
